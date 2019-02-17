#include "romExportDialog.h"
#include "ui_romExportDialog.h"


romExportDialog::romExportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::romExportDialog)
{
    ui->setupUi(this);
    m_closeEnable = true;
    m_devAddr = addrSetDialog::currentDevAddr();
    widgetInit();
    qRegisterMetaType<uint32_t>("uint32_t");
    connect(this, SIGNAL(dataExport(uint32_t,int)), this, SLOT(dataExportSlot(uint32_t,int)));

    QString str_fileName = QDate::currentDate().toString("yyyy-MM-dd.log");
    str_fileName = ROM_EXPORT_LOG_DIRECTORY + str_fileName;
    m_pLogger = Logger::LoggerCreate(str_fileName);
    if ( NULL == m_pLogger )
    {
        qDebug() << "创建日志文件失败:" << str_fileName;
    }
}

romExportDialog::~romExportDialog()
{
    if ( NULL != m_pLogger )
    {
        delete m_pLogger;
    }

    delete ui;
}

int romExportDialog::setInterface(AbsRWInterface *pInterface)
{
    m_interface = pInterface;

    return 0;
}

void* romExportDialog::threadExport(void *param)
{
    pthread_detach(pthread_self());
    romExportDialog *dialog = (romExportDialog*)param;
    uint32_t romOffset = dialog->m_dataOffset;
    uint32_t romDataLen = dialog->m_dataLen;
    QByteArray readBytes;
    int ret = dialog->infWrite(dialog->pkgExportRomAsk(romOffset, romDataLen));
    dialog->m_pLogger->writeWithDate(tr("请求导出ROM数据，起始地址：%1，总长度：%2").arg(romOffset).arg(romDataLen));
    if ( -1 == ret )
    {
        dialog->m_pLogger->writeWithDate("发送数据失败");
        dialog->dataExport(0, LOGIC_EXPORT_ERR);
        dialog->interfaceError(dialog->m_interface);
        pthread_exit(NULL);
    }
    ret = dialog->infWaitForRead(readBytes, READ_TOTAL_TIMEOUT, ROM_READ_PERIOD);
    if ( -1 == ret )
    {
        dialog->m_pLogger->writeWithDate("接收数据失败");
        dialog->dataExport(0, LOGIC_EXPORT_ERR);
        dialog->interfaceError(dialog->m_interface);
        pthread_exit(NULL);
    }
    if ( 0 == ret )
    {
        dialog->m_pLogger->writeWithDate("接收数据超时");
        dialog->dataExport(0, LOGIC_EXPORT_ERR);
        pthread_exit(NULL);
    }

    ret = dialog->parseExportRomAck(readBytes);
    if ( TP02_RTN_NO_ERR != ret )
    {
        dialog->m_pLogger->writeWithDate(tr("请求导出ROM数据失败，错误码：%1").arg(ret));
        dialog->dataExport(0, LOGIC_EXPORT_ERR_ARG);
        pthread_exit(NULL);
    }

    uint32_t readLenMax = READ_MAX_LEN;
    int sendErrorCount = SEND_ERR_COUNT;
    uint32_t readDataLen = romDataLen;
    uint32_t readOffset = romOffset;
    uint32_t readIndex = 0;

    while ( sendErrorCount-- && readDataLen)
    {
        int readLen = (readDataLen>readLenMax)?readLenMax:readDataLen;
        ret = dialog->infWrite(dialog->pkgExportRomData(readOffset, readLen));
        dialog->m_pLogger->writeWithDate(tr("请求导出数据分片，地址偏移：%1，长度：%2").arg(readOffset).arg(readLen));
        if ( -1 == ret )
        {
            dialog->m_pLogger->writeWithDate("发送数据失败");
            dialog->dataExport(readIndex, LOGIC_EXPORT_ERR);
            dialog->interfaceError(dialog->m_interface);
            pthread_exit(NULL);
        }

        ret = dialog->infWaitForRead(readBytes, READ_TOTAL_TIMEOUT, ROM_READ_PERIOD);
        if ( -1 == ret )
        {
            dialog->m_pLogger->writeWithDate("接收数据失败");
            dialog->dataExport(readIndex, LOGIC_EXPORT_ERR);
            dialog->interfaceError(dialog->m_interface);
            pthread_exit(NULL);
        }
        if ( 0 == ret )
        {
            dialog->m_pLogger->writeWithDate("接收数据超时");
            sleep(2);
            continue;
        }

        QByteArray tmpRomData;
        ret = dialog->parseExportRomData(readBytes, tmpRomData);
        if ( TP02_RTN_NO_ERR != ret )
        {
            dialog->m_pLogger->writeWithDate(tr("请求导出数据分片失败，错误码：%1").arg(ret));
            sleep(2);
            continue;
        }

        dialog->m_romDataCache.append(tmpRomData);
        readOffset += readLen;
        readIndex += readLen;
        readDataLen -= readLen;
        sendErrorCount = SEND_ERR_COUNT;
        dialog->dataExport(readIndex, LOGIC_EXPORT_DATA);
    }

    if ( readDataLen )
    {
        dialog->m_pLogger->writeWithDate(tr("导出ROM数据失败，剩余字节：%1").arg(readDataLen));
        dialog->dataExport(readIndex, LOGIC_EXPORT_ERR);
        pthread_exit(NULL);
    }

    dialog->dataExport(readIndex, LOGIC_EXPORT_FINSH);
    return (void *)NULL;
}

int romExportDialog::exportStart()
{
    int ret = pthread_create(&m_tid, NULL, threadExport, this);
    if ( ret )
    {
        perror("pthread create error!");
        return -1;
    }

    return 0;
}

QByteArray romExportDialog::pkgExportRomAsk(uint32_t offset, uint32_t dataLen)
{
    TP02Protocol tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgExportRomAsk(sendBuff, packLen, m_devAddr, offset, dataLen);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

QByteArray romExportDialog::pkgExportRomData(uint32_t offset, uint32_t dataLen)
{
    TP02Protocol tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgExportRomData(sendBuff, packLen, m_devAddr, offset, dataLen);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

int romExportDialog::parseExportRomAck(const QByteArray &ack)
{
    TP02Protocol tp;
    int ret = tp.fromRawData(ack.constData(), ack.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        return -1;
    }

    uint8_t rtn;
    ret = tp.getRtn(rtn);
    if ( TP02_RTN_NO_ERR != ret )
    {
        return -1;
    }

    return rtn;
}

int romExportDialog::parseExportRomData(const QByteArray &ack, QByteArray &data)
{
    TP02Protocol tp;
    int ret = tp.fromRawData(ack.constData(), ack.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        return -1;
    }

    char buff[4096];
    uint16_t dataLen = 0;
    ret = tp.getExportRomData(buff, dataLen);
    if ( TP02_RTN_NO_ERR != ret )
    {
        return -1;
    }

    QByteArray readByte(buff, dataLen);
    data = readByte;
    return 0;
}

int romExportDialog::saveExportData(const QByteArray &data)
{  
    if ( data.isEmpty() )
    {
        return -1;
    }

    return this->saveData2File(m_saveFileName, data);
}

int romExportDialog::saveExportCache(const QByteArray &data)
{
    if ( data.isEmpty() )
    {
        return -1;
    }

    QString cacheFileName = m_saveFileName + ".cache";
    return this->saveData2File(cacheFileName, data);
}

int romExportDialog::saveData2File(const QString &fileName, const QByteArray &data)
{
    QFile *pFile = new QFile(fileName);
    if ( !pFile->exists() )
    {
        pFile->open(QIODevice::WriteOnly | QIODevice::Text);
        pFile->close();
    }
    delete pFile;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate) )
    {
        qDebug() << "文件打开失败!";
        return -1;
    }
    file.write(data);
    file.close();
    return 0;
}

int romExportDialog::infWrite(QByteArray data)
{
    return m_interface->write(data);
}

int romExportDialog::infRead(QByteArray &data)
{
    return m_interface->read(data);
}

int romExportDialog::infWaitForRead(QByteArray &data, int msecs)
{
    return m_interface->waitForRead(data, msecs);
}

int romExportDialog::infWaitForRead(QByteArray &data, int msecs, int period)
{
    QByteArray readCache;
    int ret = m_interface->waitForRead(readCache, msecs);
    if ( ret <= 0 )
    {
        return ret;
    }
    data = readCache;

    int readedLen = m_interface->waitForRead(readCache, period);
    while ( readedLen > 0 )
    {
        ret += readedLen;
        data += readCache;
        readedLen = m_interface->waitForRead(readCache, period);
    }

    return ret;
}

void romExportDialog::setCloseEnable(bool enable)
{
    m_closeEnable = enable;
}

void romExportDialog::widgetInit()
{
    this->setWindowFlags(this->windowFlags() & (~Qt::WindowCloseButtonHint));
    ui->progressBar->setValue(0);    
}

void romExportDialog::setButtonEnable(bool enable)
{
    ui->pushButton->setEnabled(enable);
    ui->pushButton_2->setEnabled(enable);
    ui->pushButton_3->setEnabled(enable);
    ui->lineEdit->setEnabled(enable);
    ui->lineEdit_2->setEnabled(enable);
}

void romExportDialog::closeEvent(QCloseEvent *event)
{
    if ( m_closeEnable )
    {
        event->accept();
    }else{
        event->ignore();
    }

}

void romExportDialog::dataExportSlot(uint32_t readIndex, int status)
{
    switch (status) {
    case LOGIC_EXPORT_ERR:
        ui->label->setText("导出失败！");
        this->setButtonEnable(true);
        this->setCloseEnable(true);
        this->saveExportCache(m_romDataCache);
        m_saveFileName.clear();
        break;
    case LOGIC_EXPORT_FINSH:
        ui->label->setText("导出完成！");
        this->setButtonEnable(true);
        this->setCloseEnable(true);
        this->saveExportData(m_romDataCache);
        m_saveFileName.clear();
        break;
    case LOGIC_EXPORT_DATA:
        ui->progressBar->setValue(readIndex);
        ui->label->setText("数据导出中，请稍后...");
        break;
    case LOGIC_EXPORT_START:
        ui->progressBar->setMaximum(readIndex);
        ui->label->setText(tr("数据导出开始，共%1字节").arg(readIndex));
        break;
    default:
        break;
    }
}

void romExportDialog::on_pushButton_3_clicked()
{
    this->close();
}

void romExportDialog::on_pushButton_2_clicked()
{
    if ( m_saveFileName.isEmpty() )
    {
        ui->label->setText("请选择导出数据储存文件");
        return;
    }
    bool ok = false;
    m_dataOffset = ui->lineEdit->text().toUInt(&ok);
    if ( !ok )
    {
        ui->label->setText("请填写正确的数值");
        return;
    }
    m_dataLen = ui->lineEdit_2->text().toUInt(&ok);
    if ( !ok )
    {
        ui->label->setText("请填写正确的数值");
        return;
    }

    ui->progressBar->setMaximum(m_dataLen);
    m_romDataCache.clear();
    this->setButtonEnable(false);
    this->setCloseEnable(false);
    this->exportStart();
}

void romExportDialog::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "./untitled.data",
                               tr("DATA File (*.data)"));
    if( fileName.isNull() )
    {
        return ;
    }

    m_saveFileName = fileName;
}

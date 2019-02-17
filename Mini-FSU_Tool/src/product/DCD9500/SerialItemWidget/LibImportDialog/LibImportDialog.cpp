#include "LibImportDialog.h"
#include "ui_LibImportDialog.h"

LibImportDialog::LibImportDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::LibImportDialog)
{
    ui->setupUi(this);
    this->widgetInit();
    m_closeEnable = true;
    m_devAddr = addrSetDialog::currentDevAddr();
    m_comIndex = 0;
    connect(this, SIGNAL(dataSend(int)), this, SLOT(dataSendSlot(int)));

    QString str_fileName = QDate::currentDate().toString("yyyy-MM-dd.log");
    str_fileName = UPGRADE_LOG_DIRECTORY + str_fileName;
    m_pLogger = Logger::LoggerCreate(str_fileName);
    if ( NULL == m_pLogger )
    {
        qDebug() << "创建日志文件失败:" << str_fileName;
    }

}

LibImportDialog::~LibImportDialog()
{
    if ( NULL != m_pLogger )
    {
        delete m_pLogger;
    }

    delete ui;
}

int LibImportDialog::setImportComIndex(int index)
{
    m_comIndex = index;

    return 0;
}

void* LibImportDialog::threadUpgradeV2(void *param)
{
    pthread_detach(pthread_self());
    LibImportDialog* upgDialog = (LibImportDialog*)param;
    QByteArray readBytes;
    int ret;
    upgDialog->m_pDevTranrecv->recvCacheClear();
    /****************动态库导入请求开始****************/
    upgDialog->m_pLogger->writeWithDate("发送动态库导入开始指令");
    ret = upgDialog->infWrite(upgDialog->packUpgStart());
    if ( -1 == ret )
    {
        upgDialog->m_pLogger->writeWithDate("发送数据失败");
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    ret = upgDialog->infWaitForRead(readBytes, READ_TOTAL_TIMEOUT, UPGRADE_READ_PERIOD);
    if ( -1 == ret )
    {
        upgDialog->m_pLogger->writeWithDate("接收数据失败");
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    if ( 0 == ret )
    {
        upgDialog->m_pLogger->writeWithDate("接收数据超时");
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    ret = upgDialog->parseUpgAck(readBytes);
    if ( TP02_RTN_NO_ERR != ret )
    {
        qDebug() << "导入错误--导入开始请求失败,错误码：" << ret;
        upgDialog->m_pLogger->writeWithDate(tr("导入错误--导入开始请求失败,错误码：%1").arg(ret).toUtf8());
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    /***********************数据传输开始*****************/
    int byteLen = upgDialog->m_fileCache.length();
    int sendLenMax = SEND_MAX_LEN;
    int startSendIndex = 0;
    int sendErrorCount = SEND_ERR_COUNT;
    int packIndex = 1;
    upgDialog->m_pLogger->writeWithDate(tr("数据传输开始，共%1字节").arg(byteLen).toUtf8());
    while ( sendErrorCount-- && byteLen)
    {
        int sendLen = (byteLen>sendLenMax)?sendLenMax:byteLen;
        ret = upgDialog->infWrite(upgDialog->packUpgFileData(upgDialog->m_fileCache.right(byteLen).data(), sendLen, packIndex));
        upgDialog->m_pLogger->writeWithDate(tr("发送数据分片%1，共%2字节").arg(packIndex).arg(sendLen).toUtf8());
        if ( -1 == ret )
        {
            //传输出错
            upgDialog->m_pLogger->writeWithDate("发送数据失败");
            upgDialog->dataSend(LOGIC_UPGRADE_ERR);
            pthread_exit(NULL);
        }

        ret = upgDialog->infWaitForRead(readBytes, READ_TOTAL_TIMEOUT, UPGRADE_READ_PERIOD);
        if ( -1 == ret )
        {
            //传输出错
            upgDialog->m_pLogger->writeWithDate("接收数据失败");
            upgDialog->dataSend(LOGIC_UPGRADE_ERR);
            pthread_exit(NULL);
        }
        if ( 0 == ret )
        {
            //超时
            upgDialog->m_pLogger->writeWithDate("接收数据超时");
            continue;
        }

        ret = upgDialog->parseUpgAck(readBytes);
        if ( TP02_RTN_NO_ERR != ret )
        {
            qDebug() << "发送数据失败，错误码：" << ret;
            upgDialog->m_pLogger->writeWithDate(tr("发送数据失败，错误码：%1").arg(ret).toUtf8());
            sleep(2);
            continue;
        }

        byteLen -= sendLen;
        startSendIndex += sendLen;
        packIndex++;
        sendErrorCount = SEND_ERR_COUNT;
        upgDialog->dataSend(startSendIndex);
        usleep(200*1000);
    }

    if ( byteLen )
    {
        qDebug("导入错误--发送数据错误！");
        upgDialog->m_pLogger->writeWithDate(tr("导入未完成，剩余字节：%1").arg(byteLen).toUtf8());
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    /*************************动态库导入成功开始************/
    upgDialog->m_pLogger->writeWithDate("发送动态库导入完成指令");
    ret = upgDialog->infWrite(upgDialog->packUpgFinsh());
    if ( -1 == ret )
    {
        //传输出错
        upgDialog->m_pLogger->writeWithDate("发送数据失败");
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    ret = upgDialog->infWaitForRead(readBytes, READ_TOTAL_TIMEOUT, UPGRADE_READ_PERIOD);
    if ( -1 == ret )
    {
        //传输出错
        upgDialog->m_pLogger->writeWithDate("接收数据失败");
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }
    if ( 0 == ret )
    {
        //超时
        upgDialog->m_pLogger->writeWithDate("接收数据超时");
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    ret = upgDialog->parseUpgAck(readBytes);
    if ( TP02_RTN_NO_ERR != ret )
    {
        qDebug() << "导入错误--导入完成失败,错误码：" << ret;
        upgDialog->m_pLogger->writeWithDate(tr("动如错误--动态库导入完成失败,错误码：%1").arg(ret).toUtf8());
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    upgDialog->m_pLogger->writeWithDate("动态库导入完成");
    upgDialog->dataSend(LOGIC_UPGRADE_FINSH);
    return (void *)NULL;
}

int LibImportDialog::upgradeStart()
{
    int ret = pthread_create(&m_tid, NULL, threadUpgradeV2, this);
    if ( 0 != ret )
    {
        perror("pthread_create");
        return -1;
    }
    //pthread_join(m_tid, NULL);

    return 0;
}

QByteArray LibImportDialog::packUpgStart()
{
    TP02_MFSU tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgImportLibStart(sendBuff, packLen, m_devAddr, (uint8_t)m_comIndex);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

QByteArray LibImportDialog::packUpgEraseFlash()
{
    TP02Protocol tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgUpgradeErase(sendBuff, packLen, m_devAddr);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

QByteArray LibImportDialog::packUpgFileData(const char *data, uint16_t dataLen, uint16_t offset)
{
    TP02_MFSU tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgImportLibData(sendBuff, packLen, m_devAddr, data, dataLen, offset);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

QByteArray LibImportDialog::packUpgFinsh()
{
    TP02_MFSU tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgImportLibEnd(sendBuff, packLen, m_devAddr);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

QByteArray LibImportDialog::packGetSoftVer()
{
    TP02Protocol tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgGetSoftVersion(sendBuff, packLen, m_devAddr);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

int LibImportDialog::parseUpgAck(const QByteArray &ack)
{
    TP02Protocol tp;
    int ret = tp.fromRawData(ack.constData(), ack.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        return ret;
    }

    uint8_t rtn = 0;
    ret = tp.getRtn(rtn);
    if ( 0 == ret )
    {
        return rtn;
    }

    return ret;
}

int LibImportDialog::parseSoftVer(const QByteArray &ack, M3SoftVer_t &softVer)
{
    if ( TP02_RTN_NO_ERR != this->parseUpgAck(ack) )
    {
        return -1;
    }

    TP02Protocol tp;
    tp.fromRawData(ack.constData(), ack.length());
    tp.getSoftVer(softVer);

    return TP02_RTN_NO_ERR;
}

int LibImportDialog::infRead(QByteArray &data)
{
    return m_interface->read(data);
}

int LibImportDialog::infWrite(QByteArray data)
{
    return m_pDevTranrecv->appendSend(data);
}

int LibImportDialog::infWaitForRead(QByteArray &data, int msecs)
{    
    QTime time;
    time.start();
    QByteArray readCache;
    int ret = m_pDevTranrecv->readAll(readCache);
    while ( !ret && (time.elapsed() <= msecs) )
    {
        ret = m_pDevTranrecv->readAll(readCache);
        usleep(100*1000);
    }

    if ( ret < 0 )
    {
        return -1;
    }

    data = readCache;
    return readCache.length();
}

int LibImportDialog::infWaitForRead(QByteArray &data, int msecs, int period)
{
    QByteArray readCache;
    int ret =  this->infWaitForRead(readCache, msecs);
    if ( ret <= 0 )
    {
        return ret;
    }

    data = readCache;

    int readedLen = this->infWaitForRead(readCache, period);
    while ( readedLen > 0 )
    {
        ret += readedLen;
        data += readCache;
        readedLen = this->infWaitForRead(readCache, period);
    }

    return ret;
}

void LibImportDialog::setCloseEnable(bool enable)
{
    this->m_closeEnable = enable;
}

int LibImportDialog::parseFileFormat(QString fileName, M3SoftVer_t &softVer, int &devType)
{
    fileName = fileName.right(fileName.length() - fileName.lastIndexOf('/') - 1);

    for ( int i=0; i<mapDeviceTypeName.count(); i++ )
    {
        if ( !fileName.contains(mapDeviceTypeName.values().at(i)) )
        {
            continue;
        }

        QString str_format = mapDeviceTypeName.values().at(i) + "-V%d.%d.%d.bin";
        int mainVer,secVer,relVer;
        if ( 3 != sscanf(fileName.toLatin1().constData(), str_format.toLatin1().constData(),
                         &mainVer, &secVer, &relVer) )
        {
            return -1;
        }

        softVer.mainVer = mainVer;
        softVer.secVer = secVer;
        softVer.relVer = relVer;
        devType = mapDeviceTypeName.keys().at(i);
        return 0;
    }

    return -1;
}

int LibImportDialog::versionCompare(M3SoftVer_t softVer1, M3SoftVer_t softVer2)
{
    if ( softVer1.mainVer != softVer2.mainVer )
    {
        return -1;
    }

    if ( softVer1.secVer != softVer2.secVer )
    {
        return -1;
    }

    if ( softVer1.relVer != softVer2.relVer )
    {
        return -1;
    }

    return 0;
}

void LibImportDialog::widgetInit()
{
    this->setWindowTitle("动态库更新");
    this->setWindowFlags(this->windowFlags() & (~Qt::WindowCloseButtonHint));
    ui->progressBar->setValue(0);
}

void LibImportDialog::setButtonEnable(bool enable)
{
    ui->pushButton->setEnabled(enable);
    ui->pushButton_2->setEnabled(enable);
    ui->pushButton_3->setEnabled(enable);
}

void LibImportDialog::closeEvent(QCloseEvent *event)
{
    if ( m_closeEnable )
    {
        event->accept();
    }else{
        event->ignore();
    }

}

void LibImportDialog::dataSendSlot(int bytes)
{
    switch (bytes) {
    case LOGIC_UPGRADE_ERR:
        ui->label->setText("动态库更新失败！");
        this->setButtonEnable(true);
        this->setCloseEnable(true);
        break;
    case LOGIC_UPGRADE_FINSH:
        ui->label->setText("动态库更新完成！");
        this->setButtonEnable(true);
        this->setCloseEnable(true);
        break;
    case LOGIC_UPGRADE_VERSION:
        ui->label->setText("程序版本相同，已更新至最新版本！");
        this->setButtonEnable(true);
        this->setCloseEnable(true);
        break;
    default:
        ui->progressBar->setValue(bytes);
        ui->label->setText("动态库导入中中，请稍后...");
        break;
    }


}

void LibImportDialog::on_pushButton_2_clicked()
{
    if ( m_fileCache.isEmpty() )
    {
        ui->label->setText("请先选择动态库文件！");
        return;
    }

    ui->progressBar->setValue(0);
    ui->label->setText("导入开始...");
    this->setButtonEnable(false);
    this->setCloseEnable(false);
    this->upgradeStart();
}

void LibImportDialog::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),NULL,
                                                     tr("Library File (*.* *.so *.a)"));
    if( fileName.isNull() )
    {
        return ;
    }

    m_fileCache.clear();
    //int devType = 0;
    m_pLogger->writeWithDate(tr("加载文件:%1").arg(fileName).toUtf8());
/*
    if ( 0 != this->parseFileFormat(fileName, m_loadFileVer, m_comIndex) )
    {
        m_pLogger->writeWithDate("文件格式错误");
        ui->label->setText("文件格式错误！");
        return;
    }
*/
/*
    if ( devType != m_upgDevType )
    {
        m_pLogger->writeWithDate("升级包不适配");
        ui->label->setText("升级包不适配！");
        return;
    }
*/
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    if ( file.size() > 500 * 1024 )
    {
        ui->label->setText("文件过大！");
        file.close();
        return;
    }

    m_fileCache = file.readAll();
    file.close();

    ui->progressBar->setMaximum(m_fileCache.size());
    ui->label->setText("文件加载成功！");
    m_pLogger->writeWithDate("文件加载成功");
}

void LibImportDialog::on_pushButton_3_clicked()
{
    this->close();
}

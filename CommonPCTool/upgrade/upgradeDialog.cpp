#include "upgradeDialog.h"
#include "ui_upgradeDialog.h"

upgradeDialog::upgradeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::upgradeDialog)
{
    ui->setupUi(this);
    this->widgetInit();
    m_closeEnable = true;
    m_devAddr = addrSetDialog::currentDevAddr();
    m_upgDevType = TP02_DEVICE_TYPE_UNKNOW;
    connect(this, SIGNAL(dataSend(int)), this, SLOT(dataSendSlot(int)));

    QString str_fileName = QDate::currentDate().toString("yyyy-MM-dd.log");
    str_fileName = UPGRADE_LOG_DIRECTORY + str_fileName;
    m_pLogger = Logger::LoggerCreate(str_fileName);
    if ( NULL == m_pLogger )
    {
        qDebug() << "创建日志文件失败:" << str_fileName;
    }

}

upgradeDialog::~upgradeDialog()
{
    if ( NULL != m_pLogger )
    {
        delete m_pLogger;
    }

    delete ui;
}

int upgradeDialog::setInterface(AbsRWInterface *pInterface)
{
    m_interface = pInterface;

    return 0;
}

int upgradeDialog::setUpgDevType(int devType)
{
    m_upgDevType = devType;

    return 0;
}

void* upgradeDialog::threadUpgradeV2(void *param)
{
    pthread_detach(pthread_self());
    upgradeDialog* upgDialog = (upgradeDialog*)param;
    QByteArray readBytes;
    int ret;
    /*****************比较版本*******************/
    upgDialog->m_pLogger->writeWithDate("发送获取版本指令");
    ret = upgDialog->infWrite(upgDialog->packGetSoftVer());
    if ( -1 == ret )
    {
        upgDialog->m_pLogger->writeWithDate("发送数据失败");
        upgDialog->interfaceError(upgDialog->m_interface);
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }
    ret = upgDialog->infWaitForRead(readBytes, READ_TOTAL_TIMEOUT, UPGRADE_READ_PERIOD);
    if ( -1 == ret )
    {
        upgDialog->m_pLogger->writeWithDate("读取数据失败");
        upgDialog->interfaceError(upgDialog->m_interface);
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    if ( 0 == ret )
    {
        upgDialog->m_pLogger->writeWithDate("读取数据超时");
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    M3SoftVer_t softVer;
    ret = upgDialog->parseSoftVer(readBytes, softVer);
    if ( TP02_RTN_NO_ERR != ret )
    {
        qDebug() << "升级错误--获取软件版本失败,错误码：" << ret;
        upgDialog->m_pLogger->writeWithDate(tr("升级错误--获取软件版本失败,错误码:%1").arg(ret).toUtf8());
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    ret = upgDialog->versionCompare(upgDialog->m_loadFileVer, softVer);
    if ( 0 == ret )
    {
        upgDialog->m_pLogger->writeWithDate(tr("升级结束，版本号相同：V%1.%2.%3")
                                            .arg(softVer.mainVer)
                                            .arg(softVer.secVer)
                                            .arg(softVer.relVer).toUtf8());
        upgDialog->dataSend(LOGIC_UPGRADE_VERSION);
        pthread_exit(NULL);
    }


    /****************升级请求开始****************/
    upgDialog->m_pLogger->writeWithDate("发送请求升级开始指令");
    ret = upgDialog->infWrite(upgDialog->packUpgStart());
    if ( -1 == ret )
    {
        upgDialog->m_pLogger->writeWithDate("发送数据失败");
        upgDialog->interfaceError(upgDialog->m_interface);
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    ret = upgDialog->infWaitForRead(readBytes, READ_TOTAL_TIMEOUT, UPGRADE_READ_PERIOD);
    if ( -1 == ret )
    {
        upgDialog->m_pLogger->writeWithDate("接收数据失败");
        upgDialog->interfaceError(upgDialog->m_interface);
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
        qDebug() << "升级错误--升级开始请求失败,错误码：" << ret;
        upgDialog->m_pLogger->writeWithDate(tr("升级错误--升级开始请求失败,错误码：%1").arg(ret).toUtf8());
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }
    sleep(5);//等待重启
    /*********************请求擦除开始******************/
    upgDialog->m_pLogger->writeWithDate("发送请求擦除指令");
    ret = upgDialog->infWrite(upgDialog->packUpgEraseFlash());
    if ( -1 == ret )
    {
        //传输出错
        upgDialog->m_pLogger->writeWithDate("发送数据失败");
        upgDialog->interfaceError(upgDialog->m_interface);
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    ret = upgDialog->infWaitForRead(readBytes, READ_TOTAL_TIMEOUT, UPGRADE_READ_PERIOD);
    if ( -1 == ret )
    {
        //传输出错
        upgDialog->m_pLogger->writeWithDate("接收数据失败");
        upgDialog->interfaceError(upgDialog->m_interface);
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
        qDebug() << "升级错误--擦除请求失败,错误码：" << ret;
        upgDialog->m_pLogger->writeWithDate(tr("升级错误--擦除请求失败,错误码：%1").arg(ret).toUtf8());
        upgDialog->interfaceError(upgDialog->m_interface);
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
        upgDialog->m_pLogger->writeWithDate(tr("发送固件分片%1，共%2字节").arg(packIndex).arg(sendLen).toUtf8());
        if ( -1 == ret )
        {
            //传输出错
            upgDialog->m_pLogger->writeWithDate("发送数据失败");
            upgDialog->interfaceError(upgDialog->m_interface);
            upgDialog->dataSend(LOGIC_UPGRADE_ERR);
            pthread_exit(NULL);
        }

        ret = upgDialog->infWaitForRead(readBytes, READ_TOTAL_TIMEOUT, UPGRADE_READ_PERIOD);
        if ( -1 == ret )
        {
            //传输出错
            upgDialog->m_pLogger->writeWithDate("接收数据失败");
            upgDialog->interfaceError(upgDialog->m_interface);
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
        qDebug("升级错误--发送数据错误！");
        upgDialog->m_pLogger->writeWithDate(tr("升级未完成，剩余字节：%1").arg(byteLen).toUtf8());
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    /*************************升级成功开始************/
    upgDialog->m_pLogger->writeWithDate("发送升级完成指令");
    ret = upgDialog->infWrite(upgDialog->packUpgFinsh());
    if ( -1 == ret )
    {
        //传输出错
        upgDialog->m_pLogger->writeWithDate("发送数据失败");
        upgDialog->interfaceError(upgDialog->m_interface);
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    ret = upgDialog->infWaitForRead(readBytes, READ_TOTAL_TIMEOUT, UPGRADE_READ_PERIOD);
    if ( -1 == ret )
    {
        //传输出错
        upgDialog->m_pLogger->writeWithDate("接收数据失败");
        upgDialog->interfaceError(upgDialog->m_interface);
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
        qDebug() << "升级错误--升级完成失败,错误码：" << ret;
        upgDialog->m_pLogger->writeWithDate(tr("升级错误--升级完成失败,错误码：%1").arg(ret).toUtf8());
        upgDialog->dataSend(LOGIC_UPGRADE_ERR);
        pthread_exit(NULL);
    }

    upgDialog->m_pLogger->writeWithDate("升级完成");
    upgDialog->dataSend(LOGIC_UPGRADE_FINSH);
    return (void *)NULL;
}

int upgradeDialog::upgradeStart()
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

QByteArray upgradeDialog::packUpgStart()
{
    TP02Protocol tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgUpgradeStart(sendBuff, packLen, m_devAddr, m_upgDevType);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

QByteArray upgradeDialog::packUpgEraseFlash()
{
    TP02Protocol tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgUpgradeErase(sendBuff, packLen, m_devAddr);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

QByteArray upgradeDialog::packUpgFileData(const char *data, uint16_t dataLen, uint16_t offset)
{
    TP02Protocol tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgUpgradeData(sendBuff, packLen, m_devAddr, data, dataLen, offset);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

QByteArray upgradeDialog::packUpgFinsh()
{
    TP02Protocol tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgUpgradeEnd(sendBuff, packLen, m_devAddr);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

QByteArray upgradeDialog::packGetSoftVer()
{
    TP02Protocol tp;
    char sendBuff[2048] = {0};
    uint16_t packLen = 0;
    tp.genPkgGetSoftVersion(sendBuff, packLen, m_devAddr);
    QByteArray sendByte(sendBuff, packLen);

    return sendByte;
}

int upgradeDialog::parseUpgAck(const QByteArray &ack)
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

int upgradeDialog::parseSoftVer(const QByteArray &ack, M3SoftVer_t &softVer)
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

int upgradeDialog::infRead(QByteArray &data)
{
    return m_interface->read(data);
}

int upgradeDialog::infWrite(QByteArray data)
{
    if ( ui->checkBox->isChecked() )
    {
        return this->infWriteLimit(data, SEND_DATA_LIMIT, SEND_PERIOD_LIMIT);
    }

    //usleep(1000*10);
    return m_interface->write(data);
}

int upgradeDialog::infWriteLimit(QByteArray data, uint16_t sendMaxLen, uint16_t period)
{
    uint16_t sendOffset = 0;
    uint16_t packLen = data.length();
    while ( packLen )
    {
        uint16_t sendLen = (packLen>sendMaxLen) ? sendMaxLen : packLen;
        m_interface->write(data.mid(sendOffset, sendLen));
        sendOffset += sendLen;
        packLen -= sendLen;
        usleep(period);
    }

    return sendOffset;
}

int upgradeDialog::infWaitForRead(QByteArray &data, int msecs)
{    
    return m_interface->waitForRead(data, msecs);
}

int upgradeDialog::infWaitForRead(QByteArray &data, int msecs, int period)
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

void upgradeDialog::setCloseEnable(bool enable)
{
    this->m_closeEnable = enable;
}

int upgradeDialog::parseFileFormat(QString fileName, M3SoftVer_t &softVer, int &devType)
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

int upgradeDialog::versionCompare(M3SoftVer_t softVer1, M3SoftVer_t softVer2)
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

void upgradeDialog::widgetInit()
{
    this->setWindowTitle("固件更新");
    this->setWindowFlags(this->windowFlags() & (~Qt::WindowCloseButtonHint));
    ui->progressBar->setValue(0);
}

void upgradeDialog::setButtonEnable(bool enable)
{
    ui->pushButton->setEnabled(enable);
    ui->pushButton_2->setEnabled(enable);
    ui->pushButton_3->setEnabled(enable);
    ui->checkBox->setEnabled(enable);
}

void upgradeDialog::closeEvent(QCloseEvent *event)
{
    if ( m_closeEnable )
    {
        event->accept();
    }else{
        event->ignore();
    }

}

void upgradeDialog::dataSendSlot(int bytes)
{
    switch (bytes) {
    case LOGIC_UPGRADE_ERR:
        ui->label->setText("升级失败！");
        this->setButtonEnable(true);
        this->setCloseEnable(true);
        break;
    case LOGIC_UPGRADE_FINSH:
        ui->label->setText("升级完成！");
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
        ui->label->setText("升级中，请稍后...");
        break;
    }


}

void upgradeDialog::on_pushButton_2_clicked()
{
    if ( m_fileCache.isEmpty() )
    {
        ui->label->setText("请先选择升级文件！");
        return;
    }

    ui->progressBar->setValue(0);
    ui->label->setText("升级开始...");
    this->setButtonEnable(false);
    this->setCloseEnable(false);
    this->upgradeStart();
}

void upgradeDialog::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),NULL,
                                                     tr("Upgrade File (*.* *.bin)"));
    if( fileName.isNull() )
    {
        return ;
    }

    m_fileCache.clear();
    //int devType = 0;
    m_pLogger->writeWithDate(tr("加载文件:%1").arg(fileName).toUtf8());
    if ( 0 != this->parseFileFormat(fileName, m_loadFileVer, m_upgDevType) )
    {
        m_pLogger->writeWithDate("文件格式错误");
        ui->label->setText("文件格式错误！");
        return;
    }
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

void upgradeDialog::on_pushButton_3_clicked()
{
    this->close();
}

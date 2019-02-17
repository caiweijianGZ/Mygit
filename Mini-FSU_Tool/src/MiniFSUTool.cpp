#include "MiniFSUTool.h"


MiniFSUTool::MiniFSUTool(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MiniFSUTool)
{
    ui->setupUi(this);
    m_DataInit();
    loadSerialConfigFile(SERIAL_CONFIG_FILE, m_serialInfo);
    widgetInit();
    devTransceiverInit();
}

MiniFSUTool::~MiniFSUTool()
{
    delete ui;
}

int MiniFSUTool::extraFuncInit()
{
    this->menuInit();
    this->funcInit();
    this->setStackedWidgetEnable(false);
    this->setSendActionEnable(false);

    return 0;
}

int MiniFSUTool::m_DataInit()
{
    m_selProType = SERIAL_PROTOCOL_TYPE_NONE;
    m_pAbsInterface = NULL;
    m_pDevTranrecv = NULL;
    m_pWinSerialInterface = NULL;
    m_pSerialDialog = NULL;
    m_devAddr = addrSetDialog::currentDevAddr();

    return 0;
}

int MiniFSUTool::loadSerialConfigFile(const QString &fileName, winSerialInfo_t &sinfo)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly) )
    {
        qDebug() << "文件打开失败!";
        return -1;
    }

    QJsonDocument jdoc;
    QJsonParseError jerror;
    jdoc = QJsonDocument::fromJson(file.readAll(), &jerror);
    if ( jerror.error != QJsonParseError::NoError )
    {
        qDebug() << "数据格式错误1-1";
        return -1;
    }

    if ( !jdoc.isObject() )
    {
        qDebug() << "数据格式错误1-2";
        return -1;
    }

    QJsonObject jobj = jdoc.object();
    jobj = jobj.value("serialConfig").toObject();
    sinfo.baudRate = (winSerialPort::BaudRate)jobj.value("baudRate").toVariant().toInt();
    sinfo.dataBits = (winSerialPort::DataBits)jobj.value("dataBits").toVariant().toInt();
    sinfo.flowControl = (winSerialPort::FlowControl)jobj.value("flowControl").toVariant().toInt();
    sinfo.parity = (winSerialPort::Parity)jobj.value("parity").toVariant().toInt();
    sinfo.portName = jobj.value("portName").toString();
    sinfo.stopBits = (winSerialPort::StopBits)jobj.value("stopBits").toVariant().toInt();

    return 0;
}

int MiniFSUTool::saveSerialConfigToFile(const QString &fileName, const winSerialInfo_t &sinfo)
{
    QJsonObject jobj,sobj;

    sobj.insert("baudRate", sinfo.baudRate);
    sobj.insert("dataBits", sinfo.dataBits);
    sobj.insert("flowControl", sinfo.flowControl);
    sobj.insert("parity", sinfo.parity);
    sobj.insert("portName", sinfo.portName);
    sobj.insert("stopBits", sinfo.stopBits);
    jobj.insert("serialConfig", sobj);

    QJsonDocument jdoc;
    jdoc.setObject(jobj);

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
    file.write(jdoc.toJson());
    file.close();
    return 0;
}

int MiniFSUTool::devTransceiverInit()
{
    if ( NULL != m_pDevTranrecv )
    {
        return 0;
    }

    m_pDevTranrecv = new deviceTransceiver(this);
    m_pDevTranrecv->setSendPeriod(50);
    m_pDevTranrecv->setMode(deviceTransceiver::fullDuplex);
    connect(m_pDevTranrecv, SIGNAL(readyRead(QByteArray)), this, SLOT(serialReadSlot(QByteArray)));
    connect(m_pDevTranrecv, SIGNAL(interfaceError(AbsRWInterface*)), this, SLOT(interfaceErrorSlot(AbsRWInterface*)));

    return 0;
}

int MiniFSUTool::absInterfaceAttach(AbsRWInterface *absInterface)
{
    m_pAbsInterface = absInterface;
    m_pDevTranrecv->setRWInterface(m_pAbsInterface);

    return 0;
}

int MiniFSUTool::absInterfaceDetache(AbsRWInterface *absInterface)
{
    (void)absInterface;
    m_pAbsInterface = NULL;

    return 0;
}

int MiniFSUTool::widgetInit()
{
    //this->setGeometry();

    connect(ui->listWidget, SIGNAL(currentRowChanged(int)),
            ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(ui->listWidget, SIGNAL(currentTextChanged(QString)),
            ui->label, SLOT(setText(QString)));

    m_pAddrStatusLabel = new QLabel(this);
    m_pComStatusLabel = new QLabel(this);
    ui->statusBar->setStyleSheet("color:red");
    ui->statusBar->addPermanentWidget(m_pAddrStatusLabel);
    m_pAddrStatusLabel->setStyleSheet("QLabel{ color: black }");
    ui->statusBar->addPermanentWidget(m_pComStatusLabel);
    m_pComStatusLabel->setStyleSheet("QLabel{ color: black }");
    setDevAddr2StatusBar(m_devAddr);
    setComStatus2StatusBar("串口：未打开");

    return 0;
}

int MiniFSUTool::setStackedWidgetEnable(bool enable)
{
    ui->stackedWidget->setEnabled(enable);

    return 0;
}

int MiniFSUTool::setSendActionEnable(bool enable)
{
    int count = m_actionList.count();
    for ( int i=0; i<count; i++ )
    {
        m_actionList.at(i)->setEnabled(enable);
    }

    return 0;
}

int MiniFSUTool::setDevAddr2StatusBar(int addr)
{
    m_pAddrStatusLabel->setText(tr("本机地址：%1").arg(addr));

    return 0;
}

int MiniFSUTool::setComStatus2StatusBar(QString str)
{
    m_pComStatusLabel->setText(str);

    return 0;
}

int MiniFSUTool::addWidget(const QIcon &icon, QString name, QWidget *widget)
{
    ui->listWidget->addItem(new QListWidgetItem(icon, name));
    ui->stackedWidget->addWidget(widget);

    return 0;
}

int MiniFSUTool::addWidget(QString name, QWidget *widget)
{
    ui->listWidget->addItem(name);
    ui->stackedWidget->addWidget(widget);

    return 0;
}

int MiniFSUTool::funcInit()
{


    return 0;
}

int MiniFSUTool::menuInit()
{
    QMenu *menus;
    /******************设置**********************/
    menus = ui->menuBar->addMenu(tr("连接设备(&C)"));
    QAction *pAction = menus->addAction("打开串口", this, SLOT(actionSerialOpen()));
    pAction->setStatusTip("打开串口设备");

    /******************更新固件***********************/
    menus = ui->menuBar->addMenu(tr("更新(&U)"));
    pAction = menus->addAction("更新固件", this, SLOT(actionUpgrade()));
    pAction->setStatusTip("更新蓄电池固件");
    m_actionList.append(pAction);

    return 0;
}

void MiniFSUTool::serialReadSlot(QByteArray data)
{
    switch (m_selProType) {
    case SERIAL_PROTOCOL_TYPE_PMBUS:
    {
        m_serialRevCache.append(data);
        PMBusProtocol pm;
        int ret = pm.fromRawData(m_serialRevCache.data(), m_serialRevCache.length());
        if ( PMBUS_RTN_NO_ERR == ret )
        {
            m_wDialog.acceptAtOnce();
        }

    }
        break;
    case SERIAL_PROTOCOL_TYPE_TP02:
    {
        m_serialRevCache.append(data);
        TP02Protocol tp;
        int ret = tp.fromRawData(m_serialRevCache.data(), m_serialRevCache.length());
        if ( TP02_RTN_NO_ERR == ret )
        {
            m_wDialog.acceptAtOnce();
        }
    }
        break;
    case SERIAL_PROTOCOL_TYPE_NONE:
        break;
    default:
        break;
    }
}

void MiniFSUTool::interfaceErrorSlot(AbsRWInterface *intface)
{
    if ( intface == m_pWinSerialInterface )
    {
        m_pDevTranrecv->transferStop();
        m_pSerialDialog->serialClose();
        QMessageBox::warning(this, "警告", "接口异常，请检查串口是否断开连接");
        this->setSendActionEnable(false);
        this->setStackedWidgetEnable(false);
    }
}

void MiniFSUTool::actionSerialOpen()
{
    if ( NULL == m_pWinSerialInterface )
    {
        m_pWinSerialInterface = new winSerialPortInterface;
    }

    if ( NULL == m_pSerialDialog )
    {
        m_pSerialDialog = new SerialOpenDialog(m_pWinSerialInterface, m_serialInfo);
    }

    m_pDevTranrecv->transferStop();
    absInterfaceDetache(m_pWinSerialInterface);

    m_pSerialDialog->exec();

    if ( true == SerialOpenDialog::isSerialOpen() )
    {
        absInterfaceAttach(m_pWinSerialInterface);
        m_pDevTranrecv->transferStart();

        setStackedWidgetEnable(true);
        setSendActionEnable(true);

        saveSerialConfigToFile(SERIAL_CONFIG_FILE, m_serialInfo);
        setComStatus2StatusBar(tr("串口：%1, %2").arg(m_serialInfo.portName).arg(m_serialInfo.baudRate));

        return;
    }

    if ( false == SerialOpenDialog::isSerialOpen() )
    {       
        setStackedWidgetEnable(false);
        setSendActionEnable(false);
        setComStatus2StatusBar("串口：未打开");

        return;
    }
}

void MiniFSUTool::actionSetAddress()
{
    m_selProType = SERIAL_PROTOCOL_TYPE_PMBUS;
    addrSetDialog sDialog;
    sDialog.memberfromMaindow(m_pDevTranrecv, &m_wDialog, &m_serialRevCache);
    sDialog.setPMProperty(PMBUS_DEVICE_TYPE, PMBUS_VER);
    sDialog.exec();

    m_devAddr = addrSetDialog::currentDevAddr();
    setDevAddr2StatusBar(m_devAddr);
}

void MiniFSUTool::actionExportROM()
{
    romExportDialog rDialog;
    m_pDevTranrecv->transferStop();
    rDialog.setInterface(m_pAbsInterface);
    connect(&rDialog, SIGNAL(interfaceError(AbsRWInterface*)), this, SLOT(interfaceErrorSlot(AbsRWInterface*)));
    rDialog.exec();
    m_pDevTranrecv->transferStart();
}

void MiniFSUTool::actionUpgrade()
{
    m_pDevTranrecv->transferStop();
    upgradeDialog uDialog;
    uDialog.setInterface(m_pAbsInterface);
    connect(&uDialog, SIGNAL(interfaceError(AbsRWInterface*)), this, SLOT(interfaceErrorSlot(AbsRWInterface*)));
    uDialog.exec();
    m_pDevTranrecv->transferStart();
}

void MiniFSUTool::actionAbout()
{

}

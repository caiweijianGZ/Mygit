#include "SerialSetDialog.h"
#include "ui_SerialSetDialog.h"

SerialSetDialog::SerialSetDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::SerialSetDialog)
{
    ui->setupUi(this);

    QStringList headLabels;
    headLabels << "启动" << "设备Code" << "动态库" << "导入动态库";
    ui->tableWidget->setColumnCount(headLabels.count());
    ui->tableWidget->setHorizontalHeaderLabels(headLabels);
}

SerialSetDialog::~SerialSetDialog()
{
    delete ui;
}

int SerialSetDialog::loadJsonData2Ui(const QByteArray &original)
{
    QJsonParseError jerror;
    QJsonDocument jdoc = QJsonDocument::fromJson(original, &jerror);

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
    QJsonArray jarry = jobj.value(SERIAL_JSON_VALUE_SERIALS).toArray();

    return this->loadJsonData2Ui(jarry);
}

int SerialSetDialog::loadJsonData2Ui(const QJsonArray &jsonArry)
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    while ( m_serialItemList.count() )
    {
        delete m_serialItemList.takeFirst();
    }

    for ( int i=0; i<jsonArry.count(); i++ )
    {
        QJsonObject serialObj = jsonArry.at(i).toObject();
        SerialItemWidget * serialItem = new SerialItemWidget(m_pDevTranrecv, serialObj);
        this->addSerialItemWidget(ui->tableWidget, serialItem);
        m_serialItemList.append(serialItem);
    }

    return 0;
}

QByteArray SerialSetDialog::serialConfigCreate() const
{
    QJsonObject jobj, serialObj, deviceObj;
    QJsonArray serialArray, deviceArray;

    deviceObj.insert(SERIAL_JSON_VALUE_DEVCODE, "0");
    deviceObj.insert(SERIAL_JSON_VALUE_ADDR485, 1);

    deviceArray.append(deviceObj);

    serialObj.insert(SERIAL_JSON_VALUE_ACTIVE, 0);
    serialObj.insert(SERIAL_JSON_VALUE_LIBNAME, "None");
    serialObj.insert(SERIAL_JSON_VALUE_DEVICES, deviceArray);



    for ( int i=0; i<2; i++ )
    {
        serialObj.insert(SERIAL_JSON_VALUE_ID, i+1);
        serialArray.insert(i, serialObj);
    }

    jobj.insert(SERIAL_JSON_VALUE_SERIALS, serialArray);

    QJsonDocument jdoc;
    jdoc.setObject(jobj);

    return jdoc.toJson();
}

int SerialSetDialog::saveData2File(const QString &fileName, const QByteArray &data) const
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
        return -1;
    }

    file.write(data);
    file.close();

    return 0;
}

int SerialSetDialog::toJsonData(QJsonArray &jsonArry) const
{
    if ( !m_serialItemList.count() )
    {
        return -1;
    }

    QJsonArray serialArray;
    bool coverOk = false;

    for ( int i=0; i<m_serialItemList.count(); i++ )
    {
        serialArray.append(m_serialItemList.at(i)->toJsonObject(&coverOk));
        if ( false == coverOk )
        {
            return -1;
        }
    }

    jsonArry = serialArray;
    return 0;
}

int SerialSetDialog::widgetInit()
{
    return 0;
}

int SerialSetDialog::addSerialItemWidget(QTableWidget *tableWidget, SerialItemWidget *itemWidget)
{
    int insertIndex = (0 == tableWidget->rowCount()) ? 0 :  tableWidget->rowCount();
    tableWidget->insertRow(insertIndex);

    int rowIndex = tableWidget->rowCount() - 1;
    tableWidget->setCellWidget(rowIndex, 0, &itemWidget->active);
    tableWidget->setCellWidget(rowIndex, 1, &itemWidget->devCode);
    tableWidget->setCellWidget(rowIndex, 2, &itemWidget->libName);
    tableWidget->setCellWidget(rowIndex, 3, &itemWidget->libBtn);

    return 0;
}

void SerialSetDialog::on_pushButton_2_clicked()
{
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgExportSerialCfgData(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("串口配置信息获取中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose(5000))
    {
        QMessageBox::warning(this, "警告", "获取串口配置信息失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);    
    int ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取串口配置信息失败");
        return;
    }

    char chnCfgBuff[4096] = {0};
    uint16_t chnCfgBuffLen = 0;

    ret = tp.getSerialCfgData(chnCfgBuff, chnCfgBuffLen);

    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取串口配置信息失败");
        return;
    }

    QByteArray jsonBytes(chnCfgBuff, chnCfgBuffLen);
    //this->gb18030ToUtf8(QByteArray(chnCfgBuff, chnCfgBuffLen), jsonBytes);   

    ret = this->loadJsonData2Ui(jsonBytes);
    if ( 0 != ret )
    {
        QMessageBox::warning(this, "警告", "数据解析失败");
        return;
    }
    /*
    QFile file("./serialConfig.json");
    if ( file.open(QIODevice::ReadOnly) )
    {
        QByteArray jsonData = file.readAll();
        this->loadJsonData2Ui(jsonData);
        file.close();
    }
    */
}

void SerialSetDialog::on_pushButton_clicked()
{
    QJsonObject jobj;
    QJsonArray serialArry;

    int ret = this->toJsonData(serialArry);
    if ( 0 != ret )
    {
        QMessageBox::warning(this, "警告", "提交配置参数错误，请检查配置参数的格式是否正确");
        return;
    }

    jobj.insert(SERIAL_JSON_VALUE_SERIALS, serialArry);

    QJsonDocument jdoc;
    jdoc.setObject(jobj);

    char packBuff[4096] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    QByteArray utf8Bytes = jdoc.toJson(QJsonDocument::Compact);
    //QByteArray chnJsonBytes;
    //this->utf8ToGB18030(utf8Bytes, chnJsonBytes);

    tp.genPkgImportSerialCfgData(packBuff, packLen, m_devAddr, utf8Bytes.constData(), utf8Bytes.length());

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("串口配置信息发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose(5000))
    {
        QMessageBox::warning(this, "警告", "串口配置信息发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "串口配置信息发送失败");
        return;
    }

    uint8_t rtn = 0;
    ret = tp.getRtn(rtn);

    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "串口配置信息发送失败");
        return;
    }

    if ( TP02_RTN_NO_ERR != rtn )
    {
        QMessageBox::warning(this, "警告", "串口配置信息发送失败");
        return;
    }

    QMessageBox::information(this, "警告", "串口配置信息发送成功");
    //this->saveData2File("./serialConfig2.json", utf8Bytes);
}

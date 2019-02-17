#include "ChannelSetDialog.h"
#include "ui_ChannelSetDialog.h"

ChannelSetDialog::ChannelSetDialog(QWidget *parent) :
    ChannelSetDialog(NULL, parent)
{

}

ChannelSetDialog::ChannelSetDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::ChannelSetDialog)
{
    ui->setupUi(this);

    m_pAI_DICfgDialog = new AI_DICfgDialog(this);
    this->addWidget("AI/DI配置", m_pAI_DICfgDialog);

#ifndef DO_DISABLE
    m_pDOCfgDialog = new DOCfgDialog(this);
    this->addWidget("DO配置", m_pDOCfgDialog);
#endif
}

ChannelSetDialog::~ChannelSetDialog()
{
    delete ui;
}

int ChannelSetDialog::addWidget(QString tabName, QWidget *widget)
{
    ui->tabWidget->addTab(widget, tabName);

    return 0;
}

int ChannelSetDialog::channelConfigCreateCJson(QByteArray &cfgData)
{
    cJSON *AI_DIObj, *DOObj;

    AI_DIObj = cJSON_CreateObject();
    DOObj = cJSON_CreateObject();
    cJSON *signalArry = cJSON_CreateArray();
    //通用部分
    cJSON_AddNumberToObject(AI_DIObj, CHN_JSON_VALUE_ACTIVE, 0);
    cJSON_AddStringToObject(AI_DIObj, CHN_JSON_VALUE_TAG, "");
    cJSON_AddStringToObject(AI_DIObj, CHN_JSON_VALUE_DEVCODE, "0");
    cJSON_AddItemToObject(AI_DIObj, CHN_JSON_VALUE_SIGNALIDS, signalArry);

    cJSON_AddNumberToObject(DOObj, CHN_JSON_VALUE_ACTIVE, 0);
    cJSON_AddStringToObject(DOObj, CHN_JSON_VALUE_TAG, "");
    cJSON_AddStringToObject(DOObj, CHN_JSON_VALUE_DEVCODE, "0");
    cJSON_AddItemToObject(DOObj, CHN_JSON_VALUE_SIGNALIDS, signalArry);
    //DO专用
    cJSON_AddNumberToObject(AI_DIObj, CHN_JSON_VALUE_SENSOR_TYPE, 0);
    cJSON_AddNumberToObject(AI_DIObj, CHN_DI_JSON_VALUE_MAX_VALUE, 0);
    cJSON_AddNumberToObject(AI_DIObj, CHN_DI_JSON_VALUE_MIN_VALUE, 0);

    cJSON *DOArry = cJSON_CreateArray();
    cJSON *AI_DIArry = cJSON_CreateArray();

    for ( int i=0; i<2; i++ )
    {
        cJSON_AddNumberToObject(DOObj,"id", i+1);
        cJSON_AddItemToArray(DOArry, DOObj);
    }

    for ( int i=0; i<2; i++ )
    {
        cJSON_AddNumberToObject(AI_DIObj,"id", i+1);
        cJSON_AddItemToArray(AI_DIArry, AI_DIObj);
    }

    cJSON *jobj = cJSON_CreateObject();
    //cJSON_AddItemToObjectCS
    cJSON_AddItemToObject(jobj, CHN_AI_DI_JSON_VALUE_ARRAY, AI_DIArry);
    cJSON_AddItemToObject(jobj, CHN_DO_JSON_VALUE_ARRAY, DOArry);

    char *out = cJSON_Print(DOObj);
    cfgData.clear();
    cfgData.append(out, strlen(out));
    free(out);
    cJSON_Delete(DOObj);
    qDebug("cjson data:%s", out);

    return 0;
}

/*!


*/


int ChannelSetDialog::cjsonTest2(QByteArray &cfgData)
{
    cJSON *jobj = cJSON_CreateObject();
    cJSON *DOArry = cJSON_CreateArray();
    cJSON *AI_DIArry = cJSON_CreateArray();
    //cJSON_AddItemToObjectCS
    cJSON_AddItemToObject(jobj, CHN_AI_DI_JSON_VALUE_ARRAY, AI_DIArry);
    cJSON_AddItemToObject(jobj, CHN_DO_JSON_VALUE_ARRAY, DOArry);

    for ( int i=0; i<2; i++ )
    {
        cJSON *DOObj = cJSON_CreateObject();
        cJSON_AddItemToArray(DOArry, DOObj);
        cJSON_AddNumberToObject(DOObj,"id", i+1);
        cJSON_AddNumberToObject(DOObj, CHN_JSON_VALUE_ACTIVE, 0);
        cJSON_AddStringToObject(DOObj, CHN_JSON_VALUE_TAG, "");
        cJSON_AddStringToObject(DOObj, CHN_JSON_VALUE_DEVCODE, "0");
        cJSON *signalArry = cJSON_CreateArray();
        cJSON_AddItemToObject(DOObj, CHN_JSON_VALUE_SIGNALIDS, signalArry);
        cJSON_AddItemToArray(signalArry, cJSON_CreateNumber(999999999));
        cJSON_AddItemToArray(signalArry, cJSON_CreateNumber(999999999));

    }

    for ( int i=0; i<2; i++ )
    {
        cJSON *AI_DIObj = cJSON_CreateObject();
        cJSON_AddItemToArray(AI_DIArry, AI_DIObj);
        cJSON_AddNumberToObject(AI_DIObj,"id", i+1);
        cJSON_AddNumberToObject(AI_DIObj, CHN_JSON_VALUE_ACTIVE, 0);
        cJSON_AddStringToObject(AI_DIObj, CHN_JSON_VALUE_TAG, "");
        cJSON_AddStringToObject(AI_DIObj, CHN_JSON_VALUE_DEVCODE, "0");
        cJSON_AddNumberToObject(AI_DIObj, CHN_JSON_VALUE_SENSOR_TYPE, 0);
        cJSON_AddNumberToObject(AI_DIObj, CHN_DI_JSON_VALUE_MAX_VALUE, 0);
        cJSON_AddNumberToObject(AI_DIObj, CHN_DI_JSON_VALUE_MIN_VALUE, 0);
        cJSON *signalArry = cJSON_CreateArray();
        cJSON_AddItemToObject(AI_DIObj, CHN_JSON_VALUE_SIGNALIDS, signalArry);
        cJSON_AddItemToArray(signalArry, cJSON_CreateNumber(999999999));
        cJSON_AddItemToArray(signalArry, cJSON_CreateNumber(999999999));

    }

    char *out = cJSON_Print(jobj);
    qDebug("cjson data:%s\n", out);
    cJSON_Delete(jobj);
    cfgData.append(out, strlen(out));
    free(out);

    return 0;
    //qDebug("cjson data:%s", out);
}

int ChannelSetDialog::cjsonParse(QByteArray &cfgData)
{
    cJSON *json = cJSON_Parse(cfgData.constData());

    cJSON *AI_DIArray = cJSON_GetObjectItem(json, CHN_AI_DI_JSON_VALUE_ARRAY);

    if ( cJSON_Array != AI_DIArray->type )
    {
        cJSON_Delete(json);
        return -1;
    }

    int arrayCount = cJSON_GetArraySize(AI_DIArray);

    for ( int i=0; i<arrayCount; i++ )
    {
        cJSON *AI_DIObject = cJSON_GetArrayItem(AI_DIArray, i);

        cJSON *signalArray = cJSON_GetObjectItem(AI_DIObject, CHN_JSON_VALUE_SIGNALIDS);
        if ( cJSON_Array != signalArray->type )
        {
            cJSON_Delete(json);
            return -1;
        }

        int signalCount = cJSON_GetArraySize(signalArray);

        for ( int i=0; i<signalCount; i++ )
        {
            cJSON *signal = cJSON_GetArrayItem(signalArray, i);
            qDebug() << signal->valueint;
        }

    }

    cJSON_Delete(json);
    return 0;
}


int ChannelSetDialog::channelConfigCreate(QByteArray &cfgData)
{
    QJsonObject jobj, AI_DIObj, DOObj;
    QJsonArray signalArry;

    DOObj.insert(CHN_JSON_VALUE_ACTIVE, 0);
    DOObj.insert(CHN_JSON_VALUE_TAG, "");
    DOObj.insert(CHN_JSON_VALUE_DEVCODE, "0");
    DOObj.insert(CHN_JSON_VALUE_SIGNALIDS, signalArry);
    AI_DIObj = DOObj;

    AI_DIObj.insert(CHN_JSON_VALUE_SENSOR_TYPE, 0);
    AI_DIObj.insert(CHN_DI_JSON_VALUE_MAX_VALUE, 0);
    AI_DIObj.insert(CHN_DI_JSON_VALUE_MIN_VALUE, 0);

    QJsonArray DOArry, AI_DIArry;

    for ( int i=0; i<2; i++ )
    {
        DOObj.insert("id", i+1);
        DOArry.insert(i, DOObj);
    }

    for ( int i=0; i<2; i++ )
    {
        AI_DIObj.insert("id", i+1);
        AI_DIArry.insert(i, AI_DIObj);
    }

    jobj.insert(CHN_AI_DI_JSON_VALUE_ARRAY, AI_DIArry);
    jobj.insert(CHN_DO_JSON_VALUE_ARRAY, DOArry);

    QJsonDocument jdoc;
    jdoc.setObject(jobj);
    cfgData = jdoc.toJson();

    return 0;
}

int ChannelSetDialog::saveData2File(const QString &fileName, const QByteArray &data)
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

int ChannelSetDialog::gb18030ToUtf8(const QByteArray &gb18030Data, QByteArray &utf8Data)
{
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    QString unicodeStr = codec->toUnicode(gb18030Data);

    utf8Data = unicodeStr.toUtf8();
    return 0;
}

int ChannelSetDialog::utf8ToGB18030(const QByteArray &utf8Data, QByteArray &gb18030Data)
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QString unicodeStr = codec->toUnicode(utf8Data);

    codec = QTextCodec::codecForName("GB18030");
    gb18030Data = codec->fromUnicode(unicodeStr);

    return 0;
}

void ChannelSetDialog::on_pushButton_clicked()
{
    QJsonObject jobj;
    QJsonArray AI_DIArry, DOArry;

    int ret = m_pAI_DICfgDialog->getJsonData(AI_DIArry);
    if ( 0 != ret )
    {
        QMessageBox::warning(this, "警告", "提交配置参数错误，请检查配置参数的格式是否正确");
        return;
    }

    jobj.insert(CHN_AI_DI_JSON_VALUE_ARRAY, AI_DIArry);

#ifndef DO_DISABLE
    ret = m_pDOCfgDialog->getJsonData(DOArry);
    if ( 0 != ret )
    {
        QMessageBox::warning(this, "警告", "提交配置参数错误，请检查配置参数的格式是否正确");
        return;
    }
#endif

    jobj.insert(CHN_DO_JSON_VALUE_ARRAY, DOArry);

    QJsonDocument jdoc;
    jdoc.setObject(jobj);

    char packBuff[4096] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    QByteArray utf8Bytes = jdoc.toJson(QJsonDocument::Compact);
    //QByteArray chnJsonBytes;
    //this->utf8ToGB18030(utf8Bytes, chnJsonBytes);

    tp.genPkgImportChnCfgData(packBuff, packLen, m_devAddr, utf8Bytes.constData(), utf8Bytes.length());

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("通道配置信息发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose(5000))
    {
        QMessageBox::warning(this, "警告", "通道配置信息发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "通道配置信息发送失败");
        return;
    }

    uint8_t rtn = 0;
    ret = tp.getRtn(rtn);

    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "通道配置信息发送失败");
        return;
    }

    if ( TP02_RTN_NO_ERR != rtn )
    {
        QMessageBox::warning(this, "警告", "通道配置信息发送失败");
        return;
    }

    QMessageBox::information(this, "警告", "通道配置信息发送成功");
    //this->saveData2File("./channelConfig.json", jdoc.toJson());
}

void ChannelSetDialog::on_pushButton_2_clicked()
{
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgExportChnCfgData(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("通道配置信息获取中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose(5000))
    {
        QMessageBox::warning(this, "警告", "获取通道配置信息失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    int ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取通道配置信息失败");
        return;
    }

    char chnCfgBuff[4096] = {0};
    uint16_t chnCfgBuffLen = 0;
    ret = tp.getChnCfgData(chnCfgBuff, chnCfgBuffLen);

    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "通道配置信息发送失败");
        return;
    }

    QByteArray jsonBytes(chnCfgBuff, chnCfgBuffLen);
    //this->gb18030ToUtf8(QByteArray(chnCfgBuff, chnCfgBuffLen), jsonBytes);    

    ret = m_pAI_DICfgDialog->loadJsonData2Ui(jsonBytes);
    if ( 0 != ret )
    {
        QMessageBox::warning(this, "警告", "数据解析失败");
        return;
    }

#ifndef DO_DISABLE
    ret = m_pDOCfgDialog->loadJsonData2Ui(jsonBytes);
    if ( 0 != ret )
    {
        QMessageBox::warning(this, "警告", "数据解析失败");
        return;
    }
#endif
    //QMessageBox::information(this, "提示", "获取通道配置信息成功");
    /*
    QFile file("./channelConfig.json");
    if ( file.open(QIODevice::ReadOnly) )
    {
        QByteArray jsonData = file.readAll();
        m_pAI_DICfgDialog->loadJsonData2Ui(jsonData);
        m_pDOCfgDialog->loadJsonData2Ui(jsonData);
        file.close();
    }
    */
}

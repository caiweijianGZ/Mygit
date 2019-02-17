#include "CalArgsSetDialog.h"
#include "ui_CalArgsSetDialog.h"

CalArgsSetDialog::CalArgsSetDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::CalArgsSetDialog)
{
    m_devAddr = addrSetDialog::currentDevAddr();
    ui->setupUi(this);
}

CalArgsSetDialog::~CalArgsSetDialog()
{
    delete ui;
}

int CalArgsSetDialog::hex2Ascii(unsigned char *asciiData, const unsigned char *hexData, uint16_t hexLen)
{
    for ( uint16_t i=0,j=0; i<hexLen; i++,j+=2 )
    {
        uint8_t hig4   = hexData[i] >> 4;
        uint8_t low4   = hexData[i] & 0x0f;
        asciiData[j]   = (hig4 >= 0x0A) ? (hig4-0x0A+'A') : (hig4+'0');
        asciiData[j+1] = (low4 >= 0x0A) ? (low4-0x0A+'A') : (low4+'0');
    }

    return hexLen*2;
}

int CalArgsSetDialog::ascii2Hex(unsigned char *hexData, const unsigned char *asciiData, uint16_t asciiLen)
{
    for ( uint16_t i=0,j=0; i<asciiLen/2; i++,j+=2 )
    {
        uint8_t hig4 = (asciiData[j]>='A') ? (asciiData[j]-'A'+10) : (asciiData[j]-'0');
        uint8_t low4 = (asciiData[j+1]>='A') ? (asciiData[j+1]-'A'+10) : (asciiData[j+1]-'0');
        hexData[i] = (hig4 << 4) + low4;
    }

    return asciiLen/2;
}

int CalArgsSetDialog::saveData2File(const QString &fileName, const QByteArray &data)
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

void CalArgsSetDialog::on_pushButton_2_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "./untitled.cal",
                               tr("DC Cal File (*.cal)"));
    if( fileName.isNull() )
    {
        return ;
    }

    m_exportFileName = fileName;
}

void CalArgsSetDialog::on_pushButton_3_clicked()
{
    TP02_MFSU tp;
    char pack[4096] = {0};
    uint16_t len = 0;
    tp.genPkgExportMeterCalArgs(pack, len, m_devAddr);
    QByteArray sendByte(pack, len);
    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(sendByte);
    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage(tr("获取校准参数指令发送中，请稍后..."));
    if ( QDialog::Accepted != waitDialog.waitForClose() )
    {
        QMessageBox::warning(this, "警告", "获取校准参数失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);
    int ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取校准参数解析失败");
        return;
    }

    char dcArgs[4096] = {0};
    uint16_t argsLen = 0;
    ret = tp.getExportDCArgs(dcArgs, argsLen);
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", tr("获取校准参数失败:错误码：%1").arg(ret));
        return;
    }

    //char asciiArgs[4096] = {0};
    //this->hex2Ascii((unsigned char*)asciiArgs, (unsigned char*)dcArgs, argsLen);
    //this->saveData2File(m_exportFileName, QByteArray::fromRawData(asciiArgs, argsLen*2));
    this->saveData2File(m_exportFileName, QByteArray::fromRawData(dcArgs, argsLen));
    m_exportFileName.clear();
    QMessageBox::information(this, "提示", "校准参数导出成功");

}

void CalArgsSetDialog::on_pushButton_5_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),NULL,
                                                     tr("DC CAL File (*.cal)"));
    if( fileName.isNull() )
    {
        return ;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    if ( file.size() > 4 * 1024)
    {
        QMessageBox::warning(this, "警告", "导入文件过大，请选择正确参数文件");
        return;
    }

    m_importFileCache = file.readAll();
    file.close();

    QMessageBox::information(this, "提示", "文件加载成功！");
}

void CalArgsSetDialog::on_pushButton_4_clicked()
{
    //char argsHexData[1024] = {0};
    //int hexLen = this->ascii2Hex((unsigned char*)argsHexData, (unsigned char*)m_importFileCache.constData(), m_importFileCache.length());

    TP02_MFSU tp;
    char pack[4096] = {0};
    uint16_t len = 0;
    tp.genPkgImportMeterCalArgs(pack, len, m_devAddr, m_importFileCache.constData(), m_importFileCache.length());
    QByteArray sendByte(pack, len);
    m_pDevTranrecv->recvCacheClear();
    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    m_pDevTranrecv->appendSend(sendByte);
    waitDialog.setMessage(tr("导入校准参数指令发送中，请稍后..."));
    if ( QDialog::Accepted != waitDialog.waitForClose() )
    {
        QMessageBox::warning(this, "警告", "导入校准参数失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);
    int ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "导入校准参数解析失败");
        return;
    }

    uint8_t rtn = 0;
    ret = tp.getRtn(rtn);
    if ( rtn )
    {
        QMessageBox::warning(this, "警告", tr("导入校准参数解析失败，错误码：%1").arg(rtn));
        return;
    }

    m_importFileCache.clear();
    QMessageBox::information(this, "提示", "导入校准参数成功");
}


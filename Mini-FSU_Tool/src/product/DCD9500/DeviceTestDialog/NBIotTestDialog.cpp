#include "NBIotTestDialog.h"
#include "ui_NBIotTestDialog.h"

NBIotTestDialog::NBIotTestDialog(QWidget *parent) :
    NBIotTestDialog(NULL, parent)
{

}

NBIotTestDialog::NBIotTestDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::NBIotTestDialog)
{
    ui->setupUi(this);
}


NBIotTestDialog::~NBIotTestDialog()
{
    delete ui;
}

void NBIotTestDialog::on_pushButton_clicked()
{
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgTestNBIot(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("NB模块测试指令发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose(5000))
    {
        QMessageBox::warning(this, "警告", "NB模块测试指令发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    int ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "NB模块测试指令发送失败");
        return;
    }

    uint8_t rtn = 0;
    ret = tp.getRtn(rtn);

    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "NB模块测试指令发送失败");
        return;
    }

    if ( TP02_RTN_NO_ERR != rtn )
    {
        QMessageBox::warning(this, "警告", "NB模块测试指令发送失败");
        return;
    }

    QMessageBox::information(this, "警告", "NB模块测试指令发送成功");
}

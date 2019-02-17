#include "DeviceResetDialog.h"
#include "ui_DeviceResetDialog.h"

DeviceResetDialog::DeviceResetDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::DeviceResetDialog)
{
    ui->setupUi(this);
}

DeviceResetDialog::~DeviceResetDialog()
{
    delete ui;
}

void DeviceResetDialog::on_pushButton_clicked()
{
    ui->label->clear();
    if ( QMessageBox::Ok != QMessageBox::warning(this, "警告", "是否重启设备？", QMessageBox::Ok, QMessageBox::Cancel))
    {
        return;
    }

    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgDeviceReset(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("复位指令发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose(5000))
    {
        QMessageBox::warning(this, "警告", "复位指令发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    int ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "复位指令发送失败");
        return;
    }

    uint8_t rtn = 0;
    ret = tp.getRtn(rtn);

    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "复位指令发送失败");
        return;
    }

    if ( TP02_RTN_NO_ERR != rtn )
    {
        QMessageBox::warning(this, "警告", "复位指令发送失败");
        return;
    }

    QMessageBox::information(this, "警告", "复位指令发送成功");
    ui->label->setText("复位指令发送成功，请耐心等待设备重启...");
}

#include "UartTestDialog.h"
#include "ui_UartTestDialog.h"

UartTestDialog::UartTestDialog(QWidget *parent) :
    UartTestDialog(NULL, parent)
{

}

UartTestDialog::UartTestDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::UartTestDialog)
{
    ui->setupUi(this);
}

UartTestDialog::~UartTestDialog()
{
    delete ui;
}

void UartTestDialog::on_pushButton_clicked()
{
    ui->label->setText("测试中，请稍后...");
    int ret = 0;
    uint8_t rtn = 0;
    QByteArray readBytes;
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgTestUart(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("串口测试指令发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose(5000))
    {
        goto ERR;
    }

    m_pDevTranrecv->readAll(readBytes);

    ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        goto ERR;
    }

    ret = tp.getRtn(rtn);

    if ( TP02_RTN_NO_ERR != ret )
    {
        goto ERR;
    }

    if ( TP02_RTN_NO_ERR != rtn )
    {
        goto ERR;
    }

    ui->label->setStyleSheet("color:green;");
    ui->label->setText("测试通过");
    QMessageBox::information(this, "警告", "串口测试指令发送成功");

    return;

ERR:
    ui->label->setStyleSheet("color:red;");
    ui->label->setText("测试不通过");
    QMessageBox::warning(this, "警告", "串口测试指令发送失败");

}

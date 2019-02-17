#include "DOTestDialog.h"
#include "ui_DOTestDialog.h"

DOTestDialog::DOTestDialog(QWidget *parent) :
    DOTestDialog(NULL, parent)
{

}

DOTestDialog::DOTestDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::DOTestDialog)
{
    ui->setupUi(this);

    ui->lineEdit->setPlaceholderText("通道编号：[1~255]");

}

DOTestDialog::~DOTestDialog()
{
    delete ui;
}

int DOTestDialog::DOControl(int index, DOValue value)
{
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgControlChnDO(packBuff, packLen, m_devAddr, (uint8_t)index, (uint8_t)value);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("DO通道控制指令发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose(5000))
    {
        QMessageBox::warning(this, "警告", "DO通道控制指令发送失败");
        return -1;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    int ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "DO通道控制指令发送失败");
        return -1;
    }

    uint8_t rtn = 0;
    ret = tp.getRtn(rtn);

    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "DO通道控制指令发送失败");
        return -1;
    }

    if ( TP02_RTN_NO_ERR != rtn )
    {
        QMessageBox::warning(this, "警告", "DO通道控制指令发送失败");
        return -1;
    }

    QMessageBox::warning(this, "警告", "DO通道控制指令发送成功");
    return 0;
}

void DOTestDialog::on_pushButton_clicked()
{
    bool ok = false;
    int chnId = ui->lineEdit->text().toInt(&ok);

    if ( false == ok )
    {
        QMessageBox::warning(this, "警告", "请输入正确的参数");
        return;
    }

    if ( (chnId < 1) || (chnId > 255) )
    {
        QMessageBox::warning(this, "警告", "请输入正确的参数");
        return;
    }

    this->DOControl(chnId, DOValue::On);
}

void DOTestDialog::on_pushButton_2_clicked()
{
    bool ok = false;
    int chnId = ui->lineEdit->text().toInt(&ok);

    if ( false == ok )
    {
        QMessageBox::warning(this, "警告", "请输入正确的参数");
        return;
    }

    if ( (chnId < 1) || (chnId > 255) )
    {
        QMessageBox::warning(this, "警告", "请输入正确的参数");
        return;
    }

    this->DOControl(chnId, DOValue::Off);
}

void DOTestDialog::on_pushButton_3_clicked()
{
    this->DOControl(CHN_DO_INDEX_ALL, DOValue::On);
}

void DOTestDialog::on_pushButton_4_clicked()
{
    this->DOControl(CHN_DO_INDEX_ALL, DOValue::Off);
}

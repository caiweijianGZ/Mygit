#include "WorkStatusDialog.h"
#include "ui_WorkStatusDialog.h"

WorkStatusDialog::WorkStatusDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::WorkStatusDialog)
{
    ui->setupUi(this);
}

WorkStatusDialog::~WorkStatusDialog()
{
    delete ui;
}

void WorkStatusDialog::on_pushButton_clicked()
{
    ui->textBrowser->clear();

    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgGetWorkStatus(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("获取工作状态发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "获取工作状态发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    char msg[200] = {0};
    int ret = tp.getWorkStatus(msg);
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取工作状态解析失败");
        return;
    }

    ui->textBrowser->setText(QString::fromUtf8(msg));
}

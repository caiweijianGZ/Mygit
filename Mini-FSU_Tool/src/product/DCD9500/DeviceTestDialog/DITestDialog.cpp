#include "DITestDialog.h"
#include "ui_DITestDialog.h"

DITestDialog::DITestDialog(QWidget *parent) :
    DITestDialog(NULL, parent)
{

}

DITestDialog::DITestDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::DITestDialog)
{
    ui->setupUi(this);
}

DITestDialog::~DITestDialog()
{
    delete ui;
}

void DITestDialog::on_pushButton_clicked()
{
    ui->textBrowser->clear();

    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgGetChnDIData(packBuff, packLen, m_devAddr, CHN_DI_INDEX_ALL);//获取全部通道信息

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("获取DI通道数据中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose(5000))
    {
        QMessageBox::warning(this, "警告", "获取DI通道数据失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    int ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取DI通道数据失败");
        return;
    }

    char DIDataBuff[512] = {0};
    TP02DIData_t *DIData = (TP02DIData_t *)DIDataBuff;
    tp.getChnDIData(DIData);

    ui->textBrowser->append(tr("DI通道数：%1").arg(DIData->count));

    for ( int i=0; i<DIData->count; i++ )
    {
        ui->textBrowser->append(tr("通道：%1，采集量：%2")
                                .arg(DIData->data[i].id)
                                .arg(DIData->data[i].data));
    }
}

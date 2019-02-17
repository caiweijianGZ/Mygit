#include "AITestDialog.h"
#include "ui_AITestDialog.h"

AITestDialog::AITestDialog(QWidget *parent) :
    AITestDialog(NULL, parent)
{

}

AITestDialog::AITestDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::AITestDialog)
{
    ui->setupUi(this);
}

AITestDialog::~AITestDialog()
{
    delete ui;
}

void AITestDialog::on_pushButton_clicked()
{
    ui->textBrowser->clear();

    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgGetChnAIData(packBuff, packLen, m_devAddr, CHN_AI_INDEX_ALL);//获取全部通道信息

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("获取AI通道数据中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose(5000))
    {
        QMessageBox::warning(this, "警告", "获取AI通道数据失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    int ret = tp.fromRawData(readBytes.constData(), readBytes.length());
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取AI通道数据失败");
        return;
    }

    char AIDataBuff[512] = {0};
    TP02AIData_t *AIData = (TP02AIData_t *)AIDataBuff;
    tp.getChnAIData(AIData);

    //ui->textBrowser->append(tr("<<<AI原始值采集>>>"));
    ui->textBrowser->append(tr("AI通道数：%1").arg(AIData->count));

    for ( int i=0; i<AIData->count; i++ )
    {
        ui->textBrowser->append(tr("通道：%1，采集量：%2")
                                .arg(AIData->data[i].id)
                                .arg(QString::asprintf("%.4f", AIData->data[i].data / (float)10000)));
    }
}

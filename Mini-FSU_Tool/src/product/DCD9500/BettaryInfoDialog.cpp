#include "BettaryInfoDialog.h"
#include "ui_BettaryInfoDialog.h"

BettaryInfoDialog::BettaryInfoDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::BettaryInfoDialog)
{
    ui->setupUi(this);
}

BettaryInfoDialog::~BettaryInfoDialog()
{
    delete ui;
}

void BettaryInfoDialog::on_pushButton_clicked()
{
    ui->textBrowser->clear();

    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgGetBettaryInfo(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("获取电池数据发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "获取电池数据发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    Tp02BettaryInfo_t btyData;
    int ret = tp.getBettaryInfo(&btyData);
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取电池数据解析失败");
        return;
    }

    QString str_html;
    str_html.append("<html><body>");

    str_html.append(QString::asprintf("<b>电压（V）：%.3f</b><br />", btyData.vol/(float)100));
    str_html.append(QString::asprintf("<b>温度（℃）：%.3f</b><br />", btyData.temp/(float)100));
    str_html.append(tr("<b>加热状态：%1</b><br />").arg(mapHeatStatus.value(btyData.heatStatus)));
    str_html.append(tr("<b>充电状态：%1</b><br />").arg(mapChargeStatus.value(btyData.chargeStatus)));

    str_html.append("</html></body>");

    ui->textBrowser->setText(str_html);
}

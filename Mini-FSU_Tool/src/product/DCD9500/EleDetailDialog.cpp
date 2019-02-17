#include "EleDetailDialog.h"
#include "ui_EleDetailDialog.h"

EleDetailDialog::EleDetailDialog(QWidget *parent) :
    EleDetailDialog(NULL, parent)
{

}

EleDetailDialog::EleDetailDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::EleDetailDialog)
{
    ui->setupUi(this);
}

EleDetailDialog::~EleDetailDialog()
{
    delete ui;
}

void EleDetailDialog::on_pushButton_clicked()
{
    ui->textBrowser->clear();

    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgGetEleData(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("获取电量数据发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "获取电量数据发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    TP02EleData_t eleData;
    int ret = tp.getEleData(&eleData);
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取电量数据解析失败");
        return;
    }

    QString str_html;
    str_html.append("<html><body>");

    str_html.append(QString::asprintf("<b>电压（V）：%.3f</b><br />", eleData.vol/(float)1000));
    str_html.append(QString::asprintf("<b>电流（A）：%.3f</b><br />", eleData.cur/(float)1000));
    str_html.append(QString::asprintf("<b>有功功率（kW）：%.3f</b><br />", eleData.pow/(float)1000));
    str_html.append(QString::asprintf("<b>频率（HZ）：%d</b><br />", eleData.freq));
    //str_html.append(QString::asprintf("<b>当日电量（kWh）：%.3f</b><br />", eleData.eleToday/(float)1000));
    str_html.append(QString::asprintf("<b>总电量（kWh）：%.3f</b><br />", eleData.eleTotal/(float)1000));

    str_html.append("</html></body>");

    ui->textBrowser->setText(str_html);
}

void EleDetailDialog::on_pushButton_2_clicked()
{
    if ( QMessageBox::Ok != QMessageBox::warning(this, "警告", "是否清除电量数据，数据清除无法恢复", QMessageBox::Ok, QMessageBox::Cancel))
    {
        return;
    }

    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgClearEleData(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("电量数据清零指令发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "电量数据清零指令发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    uint8_t rtn = 0;
    tp.getRtn(rtn);
    if ( TP02_RTN_NO_ERR != rtn )
    {
        QMessageBox::warning(this, "警告", "电量数据清零失败");
        return;
    }

    QMessageBox::information(this, "提示", "电量数据已清零");
}

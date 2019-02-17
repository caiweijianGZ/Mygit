#include "TimeSetDialog.h"
#include "ui_TimeSetDialog.h"

TimeSetDialog::TimeSetDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::TimeSetDialog)
{
    ui->setupUi(this);
}

TimeSetDialog::~TimeSetDialog()
{
    delete ui;
}

void TimeSetDialog::TP02SetTime(const QString &time)
{
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgSetTime(packBuff, packLen, m_devAddr, time.toLocal8Bit().constData());

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("设置时间发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "设置时间发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    uint8_t rtn = 0;
    tp.getRtn(rtn);
    if ( TP02_RTN_NO_ERR != rtn )
    {
        QMessageBox::warning(this, "警告", "设置时间失败");
        return;
    }

    QMessageBox::information(this, "提示", "设置时间成功");
}

void TimeSetDialog::on_pushButton_9_clicked()
{
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgGetTime(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("获取设备时间发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "获取设备时间发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    char datetime[20] = {0};
    int ret = tp.getTime(datetime);
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取设备时间解析失败");
        return;
    }

    QString strDateTime(datetime);
    QDateTime dateTime = QDateTime::fromString(strDateTime, TIME_FORMAT);
    ui->dateTimeEdit->setDateTime(dateTime);
}

void TimeSetDialog::on_pushButton_10_clicked()
{
    QString strDateTime = ui->dateTimeEdit->dateTime().toString(TIME_FORMAT);
    this->TP02SetTime(strDateTime);
}

void TimeSetDialog::on_pushButton_11_clicked()
{
    QString strDateTime = QDateTime::currentDateTime().toString(TIME_FORMAT);
    this->TP02SetTime(strDateTime);
}

#include "DeviceDetailDialog.h"
#include "ui_DeviceDetailDialog.h"

DeviceDetailDialog::DeviceDetailDialog(QWidget *parent) :
    DeviceDetailDialog(NULL, parent)
{

}

DeviceDetailDialog::DeviceDetailDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::DeviceDetailDialog)
{
    ui->setupUi(this);
}

DeviceDetailDialog::~DeviceDetailDialog()
{
    delete ui;
}

void DeviceDetailDialog::on_pushButton_clicked()
{
    char packBuff[1024] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgGetDeviceInfo(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("获取设备信息发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "获取设备信息失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    deviceInfo_t dInfo;
    int ret = tp.getDeviceInfo(dInfo);
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取设备信息失败");
        return;
    }

    extraDevinfo_t *pExtraDevInfo = (extraDevinfo_t*)dInfo.extra;
    QString str_html;
    str_html.append("<html><body>");

    str_html.append("<p>");
    str_html.append(tr("<b>设备名称：%1</b><br />").arg(dInfo.deviceName));
    str_html.append(tr("<b>软件版本号：%1</b><br />").arg(dInfo.softwareVersion));
    str_html.append(tr("<b>存储空间：%1K</b><br />").arg(pExtraDevInfo->romSize));
    str_html.append(tr("<b>厂商名称：%1</b><br />").arg(dInfo.factoryName));
    str_html.append("</p>");

    str_html.append("</html></body>");
    ui->textBrowser->setHtml(str_html);
}

#include "OtherSetDialog.h"
#include "ui_OtherSetDialog.h"

OtherSetDialog::OtherSetDialog(QWidget *parent) :
    OtherSetDialog(NULL, parent)
{

}

OtherSetDialog::OtherSetDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::OtherSetDialog)
{
    ui->setupUi(this);

    ui->comboBox->addItems(QStringList() << mapPowerTypes.value(POWER_TYPE_UNKNOW)
                                         << mapPowerTypes.value(POWER_TYPE_DC_48V)
                                         << mapPowerTypes.value(POWER_TYPE_DC_240V)
                                         << mapPowerTypes.value(POWER_TYPE_AC_220V));
}

OtherSetDialog::~OtherSetDialog()
{
    delete ui;
}

void OtherSetDialog::on_pushButton_clicked()
{
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgGetPowType(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("获取电源配置发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "获取电源配置发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    uint8_t type = 0;
    int ret = tp.getPowerType(type);
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取电源配置发送失败");
        return;
    }

    ui->comboBox->setCurrentText(mapPowerTypes.value(type));
}

void OtherSetDialog::on_pushButton_2_clicked()
{
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    uint8_t type = mapPowerTypes.key(ui->comboBox->currentText());
    tp.genPkgSetPowType(packBuff, packLen, m_devAddr, type);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("电源配置发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "电源配置发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    uint8_t rtn = 0;
    tp.getRtn(rtn);
    if ( TP02_RTN_NO_ERR != rtn )
    {
        QMessageBox::warning(this, "警告", "修改电源配置失败");
        return;
    }

    QMessageBox::information(this, "提示", "电源配置修改成功");
}

void OtherSetDialog::on_pushButton_3_clicked()
{
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgGetFSUID(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("获取FSU设备ID发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "获取FSU设备ID发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    uint64_t fsuId;
    int ret = tp.getFSUID(fsuId);
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取FSU设备ID解析失败");
        return;
    }

    ui->lineEdit->setText(QString::number(fsuId));
}

void OtherSetDialog::on_pushButton_4_clicked()
{
    bool ok = false;
    uint64_t fsuId = ui->lineEdit->text().toLongLong(&ok);
    if ( !ok )
    {
        QMessageBox::warning(this, "警告", "请输入正确的设备ID");
        return;
    }

    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgSetFSUID(packBuff, packLen, m_devAddr, fsuId);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("FSU设备ID配置发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "FSU设备ID配置发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    uint8_t rtn = 0;
    tp.getRtn(rtn);
    if ( TP02_RTN_NO_ERR != rtn )
    {
        QMessageBox::warning(this, "警告", "修改FSU设备ID失败");
        return;
    }

    QMessageBox::information(this, "提示", "FSU设备ID配置修改成功");
}

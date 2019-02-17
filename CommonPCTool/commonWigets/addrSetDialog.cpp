#include "addrSetDialog.h"
#include "ui_addrSetDialog.h"

int addrSetDialog::m_devAddr = 1;

addrSetDialog::addrSetDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addrSetDialog)
{
    ui->setupUi(this);
    ui->lineEdit_2->setText(QString::number(m_devAddr));
    ui->lineEdit_3->setReadOnly(true);

}

addrSetDialog::~addrSetDialog()
{
    delete ui;
}

int addrSetDialog::currentDevAddr()
{
    return m_devAddr;
}

void addrSetDialog::on_pushButton_3_clicked()
{
    PMBusProtocol pm;
    char pack[1024] = {0};
    uint16_t len = 0;
    pm.genPkgGetAddress(pack, len, m_devAddr, m_PMDevType, m_PMVer);
    QByteArray sendByte(pack, len);
    m_serialRevCache->clear();
    m_devTranrecv->appendSend(sendByte);
    m_wDialog->setMessage(tr("获取设备地址指令发送中，请稍后..."));
    if ( QDialog::Accepted != m_wDialog->waitForClose(2000) )
    {
        ui->label_4->setText("获取设备地址失败");
        return;
    }

    int ret = pm.fromRawData(m_serialRevCache->constData(), m_serialRevCache->length());
    if ( PMBUS_RTN_NO_ERR != ret )
    {
        ui->label_4->setText("数据解析失败");
        return;
    }

    uint8_t addr;
    pm.getDevAddress(addr);
    ui->lineEdit_3->setText(QString::number(addr));
    ui->label_4->setText("获取设备地址成功");
}

void addrSetDialog::on_pushButton_2_clicked()
{
    if ( ui->lineEdit_2->text().isEmpty() )
    {
        ui->label_4->setText("请填写通信地址");
        return;
    }

    m_devAddr = ui->lineEdit_2->text().toInt();
    ui->label_4->setText("设置通信地址成功");
}

void addrSetDialog::on_pushButton_clicked()
{
    if ( ui->lineEdit_3->text().isEmpty() )
    {
        ui->label_4->setText("请先获取地址");
        return;
    }

    if ( ui->lineEdit->text().isEmpty() )
    {
        ui->label_4->setText("请填写设置的设备地址");
        return;
    }

    int oldAddr = ui->lineEdit_3->text().toInt();
    int newAddr = ui->lineEdit->text().toInt();
    PMBusProtocol pm;
    char pack[1024] = {0};
    uint16_t len = 0;
    pm.genPkgSetAddress(pack, len, oldAddr, newAddr, m_PMDevType, m_PMVer);
    QByteArray sendByte(pack, len);
    m_serialRevCache->clear();
    m_devTranrecv->appendSend(sendByte);
    m_wDialog->setMessage(tr("设置设备地址指令发送中，请稍后..."));
    if ( QDialog::Accepted != m_wDialog->waitForClose(2000) )
    {
        ui->label_4->setText("设置设备地址失败");
        return;
    }

    int ret = pm.fromRawData(m_serialRevCache->constData(), m_serialRevCache->length());
    if ( PMBUS_RTN_NO_ERR != ret )
    {
        ui->label_4->setText("数据解析失败");
        return;
    }

    uint8_t rtn = 0;
    ret = pm.getRtn(rtn);
    if ( PMBUS_RTN_NO_ERR != ret )
    {
        ui->label_4->setText("数据解析失败");
        return;
    }

    if ( PMBUS_RTN_NO_ERR != rtn )
    {
        ui->label_4->setText("设置设备地址失败");
    }

    ui->label_4->setText("设置设备地址成功");
}

#include "ConnectStatesDialog.h"
#include "ui_ConnectStatesDialog.h"

ConnectStatesDialog::ConnectStatesDialog(deviceTransceiver *devTrans, QWidget *parent) :
    QDialog(parent),
    TransObject(devTrans),
    ui(new Ui::ConnectStatesDialog)
{
    ui->setupUi(this);

    ui->label_2->setPixmap(QPixmap(NO_PIXMAP_PATH));
    ui->label_4->setPixmap(QPixmap(NO_PIXMAP_PATH));
}

ConnectStatesDialog::~ConnectStatesDialog()
{
    delete ui;
}

void ConnectStatesDialog::setModemConState(ConnectStatesDialog::connectState state)
{
    if ( connectState::disconnect == state )
    {
        ui->label_2->setPixmap(QPixmap(NO_PIXMAP_PATH));
    }

    if ( connectState::connect == state )
    {
        ui->label_2->setPixmap(QPixmap(YES_PIXMAP_PATH));
    }
}

void ConnectStatesDialog::setPlatformConState(ConnectStatesDialog::connectState state)
{
    if ( connectState::disconnect == state )
    {
        ui->label_4->setPixmap(QPixmap(NO_PIXMAP_PATH));
    }

    if ( connectState::connect == state )
    {
        ui->label_4->setPixmap(QPixmap(YES_PIXMAP_PATH));
    }
}

void ConnectStatesDialog::on_pushButton_clicked()
{
    char packBuff[512] = {0};
    uint16_t packLen = 0;
    TP02_MFSU tp;
    tp.genPkgGetConnectStates(packBuff, packLen, m_devAddr);

    m_pDevTranrecv->recvCacheClear();
    m_pDevTranrecv->appendSend(QByteArray(packBuff, packLen));

    waittingDialog waitDialog(m_pDevTranrecv, waittingDialog::TP02);
    waitDialog.setMessage("获取连接状态数据发送中，请稍后...");
    if ( waittingDialog::Accepted !=  waitDialog.waitForClose())
    {
        QMessageBox::warning(this, "警告", "获取连接状态数据发送失败");
        return;
    }

    QByteArray readBytes;
    m_pDevTranrecv->readAll(readBytes);

    tp.fromRawData(readBytes.constData(), readBytes.length());

    char state[10] = {0};
    int ret = tp.getConnectStates(state);
    if ( TP02_RTN_NO_ERR != ret )
    {
        QMessageBox::warning(this, "警告", "获取连接状态数据解析失败");
        return;
    }

    this->setModemConState((connectState)state[0]);
    this->setPlatformConState((connectState)state[1]);

}

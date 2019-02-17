#include "SerialOpenDialog.h"
#include "ui_SerialOpenDialog.h"

bool SerialOpenDialog::m_isSerialOpen = false;

SerialOpenDialog::SerialOpenDialog(winSerialPortInterface *serialInterface, winSerialInfo_t &sinfo, QWidget *parent) :
    QDialog(parent),
    m_pWinSerialInterface(serialInterface), m_serialInfo(sinfo),
    ui(new Ui::SerialOpenDialog)
{
    ui->setupUi(this);
    initSerialUi();
    this->serialConfigToDisplay(sinfo);
}

SerialOpenDialog::~SerialOpenDialog()
{
    delete ui;
}

bool SerialOpenDialog::isSerialOpen()
{
    return m_isSerialOpen;
}

int SerialOpenDialog::serialOpen()
{
    this->loadSerialConfigFromUI(m_serialInfo);

    int ret = m_pWinSerialInterface->open(m_serialInfo.portName);
    if ( -1 == ret )
    {
        qDebug("串口打开失败！");
        m_isSerialOpen = false;
        return -1;
    }

    ret = m_pWinSerialInterface->setSerialPort(m_serialInfo);
    if ( -1 == ret )
    {
        qDebug("串口配置失败！");
        m_isSerialOpen = false;
        return -1;
    }

    m_isSerialOpen = true;
    return 0;
}

int SerialOpenDialog::serialClose()
{
    m_isSerialOpen = false;

    return m_pWinSerialInterface->close();
}

int SerialOpenDialog::initSerialUi()
{
    this->setWindowTitle("串口配置");

    ui->pushButton->setCheckable(true);

    QStringList strl_comNum;
    strl_comNum << "COM1" << "COM2" << "COM3" << "COM4" << "COM5" << "COM6"
                << "COM7" << "COM8" << "COM9" << "COM10" << "COM11" << "COM12";
    ui->comboBox->addItems(strl_comNum);

    QStringList strl_comBaud;
    strl_comBaud << "1200" << "2400" << "4800" << "9600" << "19200" << "38400"
                 << "57600" << "115200";
    ui->comboBox_2->addItems(strl_comBaud);

    QStringList strl_dataBits;
    strl_dataBits << "8";
    ui->comboBox_3->addItems(strl_dataBits);

    QStringList strl_stopBit;
    strl_stopBit << "1";
    ui->comboBox_6->addItems(strl_stopBit);

    QStringList strl_flowControl;
    strl_flowControl << "NoFlow";
    ui->comboBox_4->addItems(strl_flowControl);

    QStringList strl_parity;
    strl_parity << "None";
    ui->comboBox_5->addItems(strl_parity);

    return 0;
}

int SerialOpenDialog::loadSerialConfigFromUI(winSerialInfo_t &sinfo)
{
    sinfo.baudRate = (winSerialPort::BaudRate)ui->comboBox_2->currentText().toULong();
    sinfo.dataBits = (winSerialPort::DataBits)ui->comboBox_3->currentText().toInt();
    sinfo.flowControl = (winSerialPort::FlowControl)ui->comboBox_4->currentIndex();
    sinfo.parity = winSerialPort::NoParity;
    sinfo.stopBits = winSerialPort::OneStop;
    sinfo.portName = ui->comboBox->currentText();

    return 0;
}

int SerialOpenDialog::serialConfigToDisplay(const winSerialInfo_t &sinfo)
{
    ui->comboBox_2->setCurrentText(QString::number((uint32_t)sinfo.baudRate));
    ui->comboBox_3->setCurrentText(QString::number((uint8_t)sinfo.dataBits));
    ui->comboBox_4->setCurrentText(QString::number((uint8_t)sinfo.flowControl));
    ui->comboBox->setCurrentText(sinfo.portName);

    return 0;
}

void SerialOpenDialog::on_pushButton_clicked()
{
    if ( "打开串口" == ui->pushButton->text() )
    {
        int ret = this->serialOpen();
        if ( 0 != ret )
        {
            ui->pushButton->setChecked(false);
            QMessageBox::warning(this, "警告", "串口打开失败");
            return;
        }

        ui->pushButton->setText("关闭串口");
        return;
    }

    if ( "关闭串口" == ui->pushButton->text() )
    {
        this->serialClose();
        ui->pushButton->setText("打开串口");
        return;
    }
}


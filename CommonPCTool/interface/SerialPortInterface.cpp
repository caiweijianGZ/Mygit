#include "SerialPortInterface.h"

SerialPortInterface::SerialPortInterface()
{

}

SerialPortInterface::SerialPortInterface(const serialInfo_t &info)
{
    this->setSerialPort(info);
}

SerialPortInterface::~SerialPortInterface()
{
    delete m_serial;
}

int SerialPortInterface::write(QByteArray data)
{
    int ret = m_serial->write(data);
    m_serial->flush();

    return ret;
}

int SerialPortInterface::waitForWrite(QByteArray data, int msecs)
{
    int ret = m_serial->write(data);
    m_serial->waitForBytesWritten(msecs);
    m_serial->flush();

    return ret;
}

int SerialPortInterface::read(QByteArray &data)
{
    data = m_serial->readAll();

    return data.length();
}

int SerialPortInterface::waitForRead(QByteArray &data, int msecs)
{
    if ( m_serial->waitForReadyRead(msecs) )
    {
        data = m_serial->readAll();
    }

    return data.length();
}

int SerialPortInterface::setSerialPort(const serialInfo_t &info)
{
    m_serial = new QSerialPort();
    m_serial->setPortName(info.portName);
    m_serial->setBaudRate(info.baudRate);
    m_serial->setStopBits(info.stopBits);
    m_serial->setParity(info.parity);
    m_serial->setDataBits(info.dataBits);
    m_serial->setFlowControl(info.flowControl);

    return 0;
}

int SerialPortInterface::open()
{
    if ( !m_serial->open(QIODevice::ReadWrite) )
    {
        return -1;
    }

    return 0;
}

int SerialPortInterface::close()
{
    m_serial->close();

    return 0;
}

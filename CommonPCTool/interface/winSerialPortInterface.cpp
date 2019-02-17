#include "winSerialPortInterface.h"

winSerialPortInterface::winSerialPortInterface()
{
    QString str_fileName = QDate::currentDate().toString("yyyy-MM-dd.log");
    str_fileName = UART_INTERFACE_LOG_DIRECTORY + str_fileName;
    m_pLogger = Logger::LoggerCreate(str_fileName);
    if ( NULL == m_pLogger )
    {
        qDebug() << "创建日志文件失败:" << str_fileName;
    }
}

winSerialPortInterface::~winSerialPortInterface()
{
    if ( NULL != m_pLogger )
    {
        delete m_pLogger;
    }

    this->close();
}

int winSerialPortInterface::write(QByteArray data)
{
    return this->waitForWrite(data);
}

int winSerialPortInterface::waitForWrite(QByteArray data, int msecs)
{
    m_pLogger->writeWithDate("[发送数据] " + data.toHex(' ').toUpper());
    return m_serial.waitForWrite(data.constData(), data.length(), msecs);
}

int winSerialPortInterface::read(QByteArray &data)
{
    return waitForRead(data);
}

int winSerialPortInterface::waitForRead(QByteArray &data, int msecs)
{
    char buff[READ_CACHE_BUFF_SIZE] = {0};
    int ret = m_serial.waitForRead(buff, READ_CACHE_BUFF_SIZE, msecs);
    if ( ret > 0 )
    {
        QByteArray readBytes(buff, ret);
        data = readBytes;
        m_pLogger->writeWithDate("[接收数据] " + data.toHex(' ').toUpper());
    }

    return ret;
}

int winSerialPortInterface::setSerialPort(const winSerialInfo_t &info)
{
    m_serial.setBaudRate(info.baudRate);
    m_serial.setDataBits(info.dataBits);
    m_serial.setParity(info.parity);
    m_serial.setStopBits(info.stopBits);
    m_serial.setFlowControl(info.flowControl);

    return 0;
}

int winSerialPortInterface::open(const QString &portName)
{
    return m_serial.open(portName.toLatin1().constData());
}

int winSerialPortInterface::close()
{
    return m_serial.close();
}

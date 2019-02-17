#include "udpInterface.h"

udpInterface::udpInterface()
{
    QString str_fileName = QDate::currentDate().toString("yyyy-MM-dd.log");
    str_fileName = UDP_INTERFACE_LOG_DIRECTORY + str_fileName;
    m_pLogger = Logger::LoggerCreate(str_fileName);
    if ( NULL == m_pLogger )
    {
        qDebug() << "创建日志文件失败:" << str_fileName;
    }
}

udpInterface::~udpInterface()
{
    if ( NULL != m_pLogger )
    {
        delete m_pLogger;
    }

    m_udpClient.udpClose();
}

int udpInterface::write(QByteArray data)
{
    m_pLogger->writeWithDate("[发送数据] " + data.toHex(' ').toUpper());
    return m_udpClient.sendMsg(data.constData(), data.length());
}

int udpInterface::waitForWrite(QByteArray data, int msecs)
{
    (void)msecs;
    m_pLogger->writeWithDate("[发送数据] " + data.toHex(' ').toUpper());
    return m_udpClient.sendMsg(data.constData(), data.length());
}

int udpInterface::read(QByteArray &data)
{
    char buff[4096] = {0};
    int rLen = m_udpClient.readMsg(buff, 4096);
    if ( -1 != rLen )
    {
        QByteArray readedBytes(buff, rLen);
        data = readedBytes;
        if ( data.length() )
        {
            m_pLogger->writeWithDate("[接收数据] " + data.toHex(' ').toUpper());
        }

        return rLen;
    }

    return -1;
}

int udpInterface::waitForRead(QByteArray &data, int msecs)
{
    char buff[4096] = {0};
    int rLen = m_udpClient.waitForReadMsg(buff, sizeof(buff), msecs);

    if ( -1 != rLen )
    {
        QByteArray readedBytes(buff, rLen);
        data = readedBytes;
        if ( data.length() )
        {
            m_pLogger->writeWithDate("[接收数据] " + data.toHex(' ').toUpper());
        }

        return rLen;
    }

    return -1;
}

int udpInterface::udpClientInit(const char *addr, uint16_t port)
{
    return m_udpClient.udpClientInit(addr, port);
}

int udpInterface::bindLocalPort(uint16_t port)
{
    return m_udpClient.udpBindPort(port);
}

int udpInterface::udpClose()
{
    return m_udpClient.udpClose();
}

#include "tcpInterface.h"

tcpInterface::tcpInterface()
{
    QString str_fileName = QDate::currentDate().toString("yyyy-MM-dd.log");
    str_fileName = TCP_INTERFACE_LOG_DIRECTORY + str_fileName;
    m_pLogger = Logger::LoggerCreate(str_fileName);
    if ( NULL == m_pLogger )
    {
        qDebug() << "创建日志文件失败:" << str_fileName;
    }
}

tcpInterface::~tcpInterface()
{
    if ( NULL != m_pLogger )
    {
        delete m_pLogger;
    }

    m_client.tcpClose();
}

int tcpInterface::write(QByteArray data)
{
    m_pLogger->writeWithDate("[发送数据] " + data.toHex(' ').toUpper());
    return m_client.sendMsg(data.constData(), data.length());
}

int tcpInterface::waitForWrite(QByteArray data, int msecs)
{
    (void)msecs;
    m_pLogger->writeWithDate("[发送数据] " + data.toHex(' ').toUpper());
    return m_client.sendMsg(data.constData(), data.length());

}

int tcpInterface::read(QByteArray &data)
{
    char buff[4096] = {0};
    int rLen = m_client.readMsg(buff, 4096);
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

int tcpInterface::waitForRead(QByteArray &data, int msecs)
{

    char buff[4096] = {0};
    int rLen = m_client.waitForReadMsg(buff, sizeof(buff), msecs);

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

int tcpInterface::tcpClientInit(const char *addr, uint16_t port)
{
    return m_client.tcpClientInit(addr, port);
}

int tcpInterface::bindLocalPort(uint16_t port)
{
    return m_client.bindLocalPort(port);
}

int tcpInterface::setNagleEnable(bool enable)
{
    return m_client.setNagleEnable(enable);
}

int tcpInterface::setKeepAlive(bool alive, const keepAliveInfo_t *kainfo)
{
    return m_client.setKeepAlive(alive, kainfo);
}

int tcpInterface::tcpConnect()
{
    return m_client.tcpConnect();
}

int tcpInterface::tcpWaitForConnect(int msecs)
{
    return m_client.tcpWaitForConnect(msecs);
}

int tcpInterface::tcpClose()
{
    return m_client.tcpClose();
}


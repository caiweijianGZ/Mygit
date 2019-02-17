#ifndef tcpInterface_H
#define tcpInterface_H

#include <QDebug>
#include <QDateTime>
#include "AbsRWInterface.h"
#include "tcp/tcpClient.h"
#include "common/Logger.h"

#define TCP_INTERFACE_LOG_DIRECTORY     "./Logs/transfer/tcp/"

class tcpInterface : public AbsRWInterface
{
public:
    tcpInterface();
    ~tcpInterface();
    virtual int write(QByteArray data);
    virtual int waitForWrite(QByteArray data, int msecs = -1);
    virtual int read(QByteArray &data);
    virtual int waitForRead(QByteArray &data, int msecs = -1);

    int tcpClientInit(const char *addr, uint16_t port);
    int bindLocalPort(uint16_t port);
    int setNagleEnable(bool enable);
    int setKeepAlive(bool alive, const keepAliveInfo_t *kainfo);
    int tcpConnect();
    int tcpWaitForConnect(int msecs = -1);
    int tcpClose();
private:
    tcpClient m_client;
    Logger *m_pLogger;
};

#endif // tcpInterface_H

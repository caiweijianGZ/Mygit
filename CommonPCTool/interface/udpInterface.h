#ifndef UDPINTERFACE_H
#define UDPINTERFACE_H

#include <QDebug>
#include "AbsRWInterface.h"
#include "udp/UdpClient.h"
#include "common/Logger.h"

#define UDP_INTERFACE_LOG_DIRECTORY     "./Logs/transfer/udp/"

class udpInterface: public AbsRWInterface
{
public:
    udpInterface();
    ~udpInterface();
    virtual int write(QByteArray data);
    virtual int waitForWrite(QByteArray data, int msecs = -1);
    virtual int read(QByteArray &data);
    virtual int waitForRead(QByteArray &data, int msecs = -1);

    int udpClientInit(const char *addr, uint16_t port);
    int bindLocalPort(uint16_t port);
    int udpClose();

private:
    UdpClient m_udpClient;
    Logger *m_pLogger;
};

#endif // UDPINTERFACE_H

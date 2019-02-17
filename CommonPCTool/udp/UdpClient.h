#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef WIN32
#include <winsock2.h>
#include <windows.h>
#include <In6addr.h>
#include <MSTCPiP.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/tcp.h>
#include <linux/types.h>
#endif

#ifndef WIN32
#define SOCKET_LEN (socklen_t *)
#else
#define SOCKET_LEN
#endif

class UdpClient
{
public:
    UdpClient();
    ~UdpClient();
    int udpClientInit(const char *addr, uint16_t port);
    int udpBindPort(int port = 0);
    int udpClose();
    int sendMsg(const char *data, int len);
    int readMsg(char *data, int len);
    int waitForReadMsg(char *data, int len, int msecs = -1);

protected:
    int m_clientFd;
    struct sockaddr_in m_remoteAddr;
    struct sockaddr_in m_localAddr;
};

#endif // UDPCLIENT_H

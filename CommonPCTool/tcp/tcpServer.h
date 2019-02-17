#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <iostream>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#ifdef WIN32
#include <In6addr.h>
#include <winsock2.h>
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

typedef struct keepAliveInfo
{
    int iKeepIdle;
    int iKeepInterval;
    int iKeepCount;
}keepAliveInfo_t;

class tcpServer
{
public:
    tcpServer();
    ~tcpServer();
    int tcpServerInit(uint32_t port);
    int setNagleEnable(bool enable);
    int setKeepAlive(bool alive, const keepAliveInfo_t *kainfo);
    int tcpAccept(struct sockaddr_in *from);
    int closeClient(int sockFd);
    int closeServer();
    int sendMsg(int sockFd, const char *data, int len);
    int readMsg(int sockFd, char *data, int len);

protected:
    int m_localSocketFd;

};

#endif // TCPSERVER_H

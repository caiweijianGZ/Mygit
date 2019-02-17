#include "tcpserver.h"

tcpServer::tcpServer()
{
    m_localSocketFd = 0;
}

tcpServer::~tcpServer()
{
    this->closeServer();
}

int tcpServer::tcpServerInit(uint32_t port)
{
#ifdef WIN32
    WSADATA w;
    if (WSAStartup(MAKEWORD(1, 1), &w))
    {
        return -1;
    }
#endif
    m_localSocketFd = socket(AF_INET , SOCK_STREAM, 0);

#ifndef WIN32
    int val = fcntl(m_localSocketFd, F_GETFD);
    val |= FD_CLOEXEC;
    fcntl(m_localSocketFd, F_SETFD, val);
#endif
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    char on = 1;
    setsockopt(m_localSocketFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int ret = bind(m_localSocketFd, (struct sockaddr *)&addr, sizeof(addr));
    if ( -1 == ret )
    {
        perror("bind\n");
        close(m_localSocketFd);

        return -1;
    }
    ret = listen(m_localSocketFd, 1);
    if ( -1 == ret )
    {
        perror("listen\n");
        close(m_localSocketFd);

        return -1;
    }

    return 0;
}

int tcpServer::setNagleEnable(bool enable)
{
    if ( !enable )
    {
        char iMode = 1;
        setsockopt(m_localSocketFd, IPPROTO_TCP, TCP_NODELAY, &iMode, sizeof(iMode));
    }

    return 0;
}

int tcpServer::setKeepAlive(bool alive, const keepAliveInfo_t *kainfo)
{
    char iMode = 0;
    if ( !alive )
    {
        setsockopt(m_localSocketFd, SOL_SOCKET, SO_KEEPALIVE, &iMode, sizeof(iMode));
        return 0;
    }

    iMode = 1;
#ifdef WIN32
    DWORD dw;
    tcp_keepalive sKA_Settings, sReturned;
    sKA_Settings.onoff = 1 ;
    sKA_Settings.keepalivetime = kainfo->iKeepIdle ; // msecs
    sKA_Settings.keepaliveinterval = kainfo->iKeepInterval ; // msecs

    WSAIoctl(m_localSocketFd, SIO_KEEPALIVE_VALS, &sKA_Settings,
         sizeof(sKA_Settings), &sReturned, sizeof(sReturned), &dw,
         NULL, NULL);
#else
    setsockopt(m_localSocketFd, IPPROTO_TCP, TCP_NODELAY, (const void *)&iMode, sizeof(iMode));
    setsockopt(m_localSocketFd, SOL_SOCKET, SO_KEEPALIVE, (const void *)&iMode, sizeof(iMode));

    setsockopt(m_localSocketFd, IPPROTO_TCP, TCP_KEEPIDLE, (const void *)&kainfo->iKeepIdle, sizeof(int));
    setsockopt(m_localSocketFd, IPPROTO_TCP, TCP_KEEPINTVL, (const void *)&kainfo->iKeepInterval, sizeof(int));
    setsockopt(m_localSocketFd, IPPROTO_TCP, TCP_KEEPCNT, (const void *)&kainfo->iKeepCount, sizeof(int));
#endif
    return 0;
}

int tcpServer::tcpAccept(struct sockaddr_in *from)
{
    int len = sizeof(struct sockaddr_in);
    return accept(m_localSocketFd, (struct sockaddr *)from, SOCKET_LEN&len);
}

int tcpServer::closeClient(int sockFd)
{
#ifdef WIN32
    shutdown(sockFd, SD_BOTH);
#else
    shutdown(sockFd, SHUT_RDWR);
#endif
    close(sockFd);

    return 0;
}

int tcpServer::closeServer()
{
    if ( 0 != m_localSocketFd )
    {
        close(m_localSocketFd);
    }

    return 0;
}

int tcpServer::sendMsg(int sockFd, const char *data, int len)
{
#ifdef WIN32
    return send(sockFd, data, len, 0);
#else
    return send(sockFd, data, len, MSG_NOSIGNAL);
#endif
}

int tcpServer::readMsg(int sockFd, char *data, int len)
{
    return recv(sockFd, data, len, 0);
}




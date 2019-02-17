#include "tcpClient.h"

tcpClient::tcpClient()
{
    m_clientFd = 0;
}

tcpClient::~tcpClient()
{
    this->tcpClose();
}

int tcpClient::tcpClientInit(const char *addr, uint16_t port)
{
#ifdef WIN32
    WSADATA w;
    if (WSAStartup(MAKEWORD(1, 1), &w))
    {
        return -1;
    }
#endif
    m_clientFd = socket(AF_INET , SOCK_STREAM, 0);

#ifndef WIN32
    int val = fcntl(m_clientFd, F_GETFD);
    val |= FD_CLOEXEC;
    fcntl(m_clientFd, F_SETFD, val);
#endif

    char on = 1;
    setsockopt(m_clientFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    memset(&m_remoteAddr, 0, sizeof(m_remoteAddr));
    m_remoteAddr.sin_family = AF_INET;
    m_remoteAddr.sin_port = htons(port);
    m_remoteAddr.sin_addr.s_addr = inet_addr(addr);

    return 0;
}

int tcpClient::bindLocalPort(uint16_t port)
{
    struct sockaddr_in localAddr;
    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port = htons(port);
    localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    return bind(m_clientFd, (struct sockaddr *)&localAddr, sizeof(struct sockaddr_in));
}

int tcpClient::setNagleEnable(bool enable)
{
    if ( !enable )
    {
        char iMode = 1;
        setsockopt(m_clientFd, IPPROTO_TCP, TCP_NODELAY, &iMode, sizeof(iMode));
    }

    return 0;
}

int tcpClient::setKeepAlive(bool alive, const keepAliveInfo_t *kainfo)
{
    char iMode = 0;
    if ( !alive )
    {
        setsockopt(m_clientFd, SOL_SOCKET, SO_KEEPALIVE, &iMode, sizeof(iMode));
        return 0;
    }

    iMode = 1;
#ifdef WIN32
    DWORD dw;
    tcp_keepalive sKA_Settings, sReturned;
    sKA_Settings.onoff = 1 ;
    sKA_Settings.keepalivetime = kainfo->iKeepIdle ; // msecs
    sKA_Settings.keepaliveinterval = kainfo->iKeepInterval ; // msecs

    WSAIoctl(m_clientFd, SIO_KEEPALIVE_VALS, &sKA_Settings,
         sizeof(sKA_Settings), &sReturned, sizeof(sReturned), &dw,
         NULL, NULL);
#else
    setsockopt(m_clientFd, SOL_SOCKET, SO_KEEPALIVE, (const void *)&iMode, sizeof(iMode));

    setsockopt(m_clientFd, IPPROTO_TCP, TCP_KEEPIDLE, (const void *)&kainfo->iKeepIdle, sizeof(int));
    setsockopt(m_clientFd, IPPROTO_TCP, TCP_KEEPINTVL, (const void *)&kainfo->iKeepInterval, sizeof(int));
    setsockopt(m_clientFd, IPPROTO_TCP, TCP_KEEPCNT, (const void *)&kainfo->iKeepCount, sizeof(int));
#endif
    return 0;
}

int tcpClient::tcpConnect()
{
    return connect(m_clientFd, (struct sockaddr *)&m_remoteAddr, sizeof(struct sockaddr_in));
}

int tcpClient::tcpWaitForConnect(int msecs)
{
    u_long mode = 1;
    ioctlsocket(m_clientFd, FIONBIO, &mode);//先设置成非阻塞模式

    int ret = connect(m_clientFd, (struct sockaddr *)&m_remoteAddr, sizeof(struct sockaddr_in));
    if ( 0 == ret )
    {
        mode = 0;
        ioctlsocket(m_clientFd, FIONBIO, &mode);//设置成阻塞模式
        return 0;
    }

    fd_set fdWrite;
    FD_ZERO(&fdWrite);
    FD_SET(m_clientFd, &fdWrite);
    int nRet = 0;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000*msecs;
#ifdef WIN32
    nRet = select(0, NULL, &fdWrite, NULL, &tv);//是否可写
#else
    nRet = select( m_clientFd + 1, &fdWrite, NULL, NULL, &tv);
#endif

    mode = 0;
    ioctlsocket(m_clientFd, FIONBIO, &mode);//设置成阻塞模式

    if ( nRet <= 0 )
    {
        return -1;
    }
    int error;
    int len = sizeof(error);
    getsockopt(m_clientFd, SOL_SOCKET, SO_ERROR, (char*)&error, SOCKET_LEN&len);

    return error;
}

int tcpClient::tcpClose()
{
    if ( m_clientFd )
    {
#ifdef WIN32
        closesocket(m_clientFd);
#else
        close(m_clientFd);
#endif
        m_clientFd = 0;
    }

    return 0;
}

int tcpClient::sendMsg(const char *data, int len)
{
#ifdef WIN32
    return send(m_clientFd, data, len, 0);
#else
    return send(m_clientFd, data, len, MSG_NOSIGNAL);
#endif
}

int tcpClient::readMsg(char *data, int len)
{
    return recv(m_clientFd, data, len, 0);
}

int tcpClient::waitForReadMsg(char *data, int len, int msecs)
{
    fd_set fdRead;
    FD_ZERO(&fdRead);
    FD_SET(m_clientFd, &fdRead);
    int nRet = 0;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 1000*msecs;
#ifdef WIN32
    nRet = select(0, &fdRead, NULL, NULL, &tv);
#else
    nRet = select( m_clientFd + 1, &fdRead, NULL, NULL, &tv);
#endif

    if ( 0 == nRet )
    {
        return nRet;
    }

    nRet = recv(m_clientFd, data, len, 0);
    //0表示连接正常关闭，-1表示连接非正常断开，大于0表示实际接收的字节，为了方便处理，0和-1都认为断开
    return (0==nRet)?-1:nRet;
}

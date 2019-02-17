#include "UdpClient.h"

UdpClient::UdpClient()
{
    m_clientFd = 0;
}

UdpClient::~UdpClient()
{
    this->udpClose();
}

int UdpClient::udpClientInit(const char *addr, uint16_t port)
{

#ifdef WIN32
    WSADATA w;
    if (WSAStartup(MAKEWORD(1, 1), &w))
    {
        return -1;
    }
#endif
    m_clientFd = socket(AF_INET , SOCK_DGRAM, 0);

#ifndef WIN32
    int val = fcntl(m_clientFd, F_GETFD);
    val |= FD_CLOEXEC;
    fcntl(m_clientFd, F_SETFD, val);
#endif

    //char on = 1;
    //setsockopt(m_clientFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    memset(&m_remoteAddr, 0, sizeof(m_remoteAddr));
    m_remoteAddr.sin_family = AF_INET;
    m_remoteAddr.sin_port = htons(port);
    m_remoteAddr.sin_addr.s_addr = inet_addr(addr);

    return 0;
}

int UdpClient::udpBindPort(int port)
{
    memset(&m_localAddr,0,sizeof(m_localAddr));
    m_localAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    m_localAddr.sin_family = AF_INET;
    m_localAddr.sin_port = htons(port); //指定随机端口

    int ret = bind(m_clientFd,(SOCKADDR*)&m_localAddr,sizeof(SOCKADDR)); //把socket 绑定到指定地址上
    if( ret != 0 )
    {
       return -1;
    }

    int len = sizeof(m_localAddr);
    ret = getsockname(m_clientFd, (SOCKADDR*)&m_localAddr, &len);
    if ( 0 != ret )
    {
        return -1;
    }

    return ntohs(m_localAddr.sin_port);
}

int UdpClient::udpClose()
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

int UdpClient::sendMsg(const char *data, int len)
{
    return sendto(m_clientFd, data, len, 0, (struct sockaddr *)&m_remoteAddr, sizeof(struct sockaddr_in));
}

int UdpClient::readMsg(char *data, int len)
{
    struct sockaddr_in recvAddr;
    int length = sizeof(struct sockaddr_in);
    return recvfrom(m_clientFd, data, len, 0, (struct sockaddr *)&recvAddr, &length);
}

int UdpClient::waitForReadMsg(char *data, int len, int msecs)
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

    struct sockaddr_in recvAddr;
    int length = sizeof(struct sockaddr_in);
    nRet = recvfrom(m_clientFd, data, len, 0, (struct sockaddr *)&recvAddr, &length);
    //0表示连接正常关闭，-1表示连接非正常断开，大于0表示实际接收的字节，为了方便处理，0和-1都认为断开
    return (0==nRet)?-1:nRet;
}

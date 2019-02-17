#include "winSerialPort.h"

winSerialPort::winSerialPort()
{

}

winSerialPort::~winSerialPort()
{
    this->close();
}

int winSerialPort::write(const char *data, int len)
{
    return waitForWrite(data, len);
}

int winSerialPort::waitForWrite(const char *data, int len, int msecs)
{
    BOOL  bResult = TRUE;
    DWORD  BytesToSend = 0;
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    COMMTIMEOUTS  CommTimeouts;
    bResult = GetCommTimeouts(m_hComm, &CommTimeouts);
    if ( FALSE == bResult )
    {
        return -1;
    }

    CommTimeouts.WriteTotalTimeoutMultiplier = 0;
    CommTimeouts.WriteTotalTimeoutConstant = (-1==msecs)?0:msecs;
    bResult = SetCommTimeouts(m_hComm, &CommTimeouts);
    if ( FALSE == bResult )
    {
        return -1;
    }

    bResult = WriteFile(m_hComm, data, len, &BytesToSend, NULL);
    if ( FALSE == bResult )
    {
        PurgeComm(m_hComm, PURGE_TXCLEAR | PURGE_TXABORT);
#if 0
        DWORD dwError = 0;  /** 错误码 */
        COMSTAT  comstat;   /** COMSTAT结构体,记录通信设备的状态信息 */
        memset(&comstat, 0, sizeof(COMSTAT));

        /** 在调用ReadFile和WriteFile之前,通过本函数清除以前遗留的错误标志 */
        ClearCommError(m_hComm, &dwError, &comstat);
#endif
    }

    return BytesToSend;
}

int winSerialPort::read(char *data, int len)
{
    return waitForRead(data, len);
}

int winSerialPort::waitForRead(char *data, int len, int msecs)
{
    BOOL  bResult = TRUE;
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    if ( len > READ_CACHE_BUFF_SIZE )
    {
        return -1;
    }

    COMMTIMEOUTS  CommTimeouts;
    bResult = GetCommTimeouts(m_hComm, &CommTimeouts);

    CommTimeouts.ReadIntervalTimeout = READ_INTERVAL_TIMEOUT;//MAXDWORD表示立刻把缓冲区的数据返回
    CommTimeouts.ReadTotalTimeoutMultiplier = 0;
    CommTimeouts.ReadTotalTimeoutConstant = (-1==msecs)?0:msecs;
    bResult = SetCommTimeouts(m_hComm, &CommTimeouts);

    if ( FALSE == bResult )
    {
        return -1;
    }

    DWORD BytesRead = 0;
    bResult = ReadFile(m_hComm, data, len, &BytesRead, NULL);
    if ( !bResult )
    {
        PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_RXABORT);
        return -1;
    }

    return BytesRead;
}

int winSerialPort::setBaudRate(winSerialPort::BaudRate baudRate)
{
    DCB  dcb;
    memset(&dcb, 0, sizeof(DCB));
    BOOL bIsSuccess = TRUE;
    bIsSuccess = GetCommState(m_hComm, &dcb);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    dcb.BaudRate = baudRate;
    bIsSuccess = SetCommState(m_hComm, &dcb);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    return 0;
}

int winSerialPort::setDataBits(winSerialPort::DataBits dataBits)
{
    DCB  dcb;
    memset(&dcb, 0, sizeof(DCB));
    BOOL bIsSuccess = TRUE;
    bIsSuccess = GetCommState(m_hComm, &dcb);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    dcb.ByteSize = dataBits;
    bIsSuccess = SetCommState(m_hComm, &dcb);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    return 0;
}

int winSerialPort::setStopBits(winSerialPort::StopBits stopBits)
{
    DCB  dcb;
    memset(&dcb, 0, sizeof(DCB));
    BOOL bIsSuccess = TRUE;
    bIsSuccess = GetCommState(m_hComm, &dcb);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    dcb.StopBits = stopBits;
    bIsSuccess = SetCommState(m_hComm, &dcb);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    return 0;
}

int winSerialPort::setParity(winSerialPort::Parity parity)
{
    DCB  dcb;
    memset(&dcb, 0, sizeof(DCB));
    BOOL bIsSuccess = TRUE;
    bIsSuccess = GetCommState(m_hComm, &dcb);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    dcb.Parity = parity;
    bIsSuccess = SetCommState(m_hComm, &dcb);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    return 0;
}

int winSerialPort::setFlowControl(winSerialPort::FlowControl flowControl)
{
    (void)flowControl;
    return 0;
}

int winSerialPort::open(const char *portName)
{
    m_hComm = CreateFileA(portName,  /** 设备名,COM1,COM2等 */
        GENERIC_READ | GENERIC_WRITE, /** 访问模式,可同时读写 */
        0,                            /** 共享模式,0表示不共享 */
        NULL,                         /** 安全性设置,一般使用NULL */
        OPEN_EXISTING,                /** 该参数表示设备必须存在,否则创建失败 */
        0,                            /** FILE_FLAG_OVERLAPPED异步 */
        0);

    /** 如果打开失败，释放资源并返回 */
    if (m_hComm == INVALID_HANDLE_VALUE)
    {
        return -1;
    }

    char defaultParam[50];
    sprintf(defaultParam, "baud=%d parity=%c data=%d stop=%d", 9600, 'N', 8, 1);
    DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, defaultParam, -1, NULL, 0);
    wchar_t *pwText = new wchar_t[dwNum];
    MultiByteToWideChar(CP_ACP, 0, defaultParam, -1, pwText, dwNum);

    DCB  dcb;
    memset(&dcb, 0, sizeof(DCB));
    BOOL bIsSuccess = TRUE;
    bIsSuccess = BuildCommDCB(pwText, &dcb);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    bIsSuccess = SetCommState(m_hComm, &dcb);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    bIsSuccess = PurgeComm(m_hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    bIsSuccess = SetupComm(m_hComm,READ_CACHE_BUFF_SIZE,SEND_CACHE_BUFF_SIZE);
    if ( FALSE == bIsSuccess )
    {
        return -1;
    }

    return 0;
}

int winSerialPort::close()
{
    if (m_hComm != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
    }

    return 0;
}

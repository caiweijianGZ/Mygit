#ifndef WINSERIALPORTINTERFACE_H
#define WINSERIALPORTINTERFACE_H

#include "serial/winSerialPort.h"
#include "AbsRWInterface.h"
#include "common/Logger.h"

#define UART_INTERFACE_LOG_DIRECTORY     "./Logs/transfer/uart/"

typedef struct winSerialInfo
{
    winSerialPort::BaudRate baudRate;
    winSerialPort::StopBits stopBits;
    winSerialPort::Parity parity;
    winSerialPort::DataBits dataBits;
    winSerialPort::FlowControl flowControl;
    QString portName;
}winSerialInfo_t;

class winSerialPortInterface : public AbsRWInterface
{
public:
    winSerialPortInterface();
    ~winSerialPortInterface();
    virtual int write(QByteArray data);
    virtual int waitForWrite(QByteArray data, int msecs = -1);
    virtual int read(QByteArray &data);
    virtual int waitForRead(QByteArray &data, int msecs = -1);

    int setSerialPort(const winSerialInfo_t &info);
    int open(const QString &portName);
    int close();

private:    
    winSerialPort  m_serial;
    Logger *m_pLogger;
};

#endif // WINSERIALPORTINTERFACE_H

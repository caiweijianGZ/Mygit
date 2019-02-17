#ifndef WINSERIALPORT_H
#define WINSERIALPORT_H

#include <qt_windows.h>
#include "stdio.h"

#define READ_INTERVAL_TIMEOUT               100
#define READ_CACHE_BUFF_SIZE                4096
#define SEND_CACHE_BUFF_SIZE                4096



class winSerialPort
{
public:
    enum BaudRate {
        Baud1200 = CBR_1200,
        Baud2400 = CBR_2400,
        Baud4800 = CBR_4800,
        Baud9600 = CBR_9600,
        Baud19200 = CBR_19200,
        Baud38400 = CBR_38400,
        Baud57600 = CBR_57600,
        Baud115200 = CBR_115200,
        Baud128000 = CBR_128000,
        Baud256000 = CBR_256000,
        UnknownBaud = -1
    };

    enum DataBits {
        Data5 = 5,
        Data6 = 6,
        Data7 = 7,
        Data8 = 8,
        UnknownDataBits = -1
    };

    enum StopBits {
        OneStop = 0,
        OneAndHalfStop = 1,
        TwoStop = 2,
        UnknownStopBits = -1
    };

    enum Parity {
        NoParity = 0,
        OddParity = 1,
        EvenParity = 2,
        MarkParity = 3,
        SpaceParity = 4,
        UnknownParity = -1
    };

    enum FlowControl {
        NoFlowControl,
        HardwareControl,
        SoftwareControl,
        UnknownFlowControl = -1
    };

    winSerialPort();
    ~winSerialPort();
    int write(const char *data, int len);
    int waitForWrite(const char *data, int len, int msecs = -1);
    int read(char *data, int len);
    int waitForRead(char *data, int len, int msecs = -1);

    int setBaudRate(winSerialPort::BaudRate baudRate);
    int setDataBits(winSerialPort::DataBits dataBits);
    int setStopBits(winSerialPort::StopBits stopBits);
    int setParity(winSerialPort::Parity parity);
    int setFlowControl(winSerialPort::FlowControl flowControl);
    int open(const char *portName);
    int close();

private:
    HANDLE  m_hComm;
};

#endif // winSerialPort_H

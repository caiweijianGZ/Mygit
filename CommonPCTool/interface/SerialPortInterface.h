#ifndef SERIALPORTINTERFACE_H
#define SERIALPORTINTERFACE_H

#include <QSerialPort>
#include <QDebug>
#include "AbsRWInterface.h"


typedef struct serialInfo
{
    QSerialPort::BaudRate baudRate;
    QSerialPort::StopBits stopBits;
    QSerialPort::Parity parity;
    QSerialPort::DataBits dataBits;
    QSerialPort::FlowControl flowControl;
    QString portName;
}serialInfo_t;

class SerialPortInterface : public AbsRWInterface
{
public:
    SerialPortInterface();
    explicit SerialPortInterface(const serialInfo_t &info);
    ~SerialPortInterface();
    virtual int write(QByteArray data);
    virtual int waitForWrite(QByteArray data, int msecs = -1);
    virtual int read(QByteArray &data);
    virtual int waitForRead(QByteArray &data, int msecs = -1);

    int setSerialPort(const serialInfo_t &info);
    int open();
    int close();

private:
    QSerialPort *m_serial;
};

#endif // SERIALPORTINTERFACE_H

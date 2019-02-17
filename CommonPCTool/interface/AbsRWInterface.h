#ifndef ABSRWINTERFACE_H
#define ABSRWINTERFACE_H

#include <QObject>

class AbsRWInterface
{
public:
    virtual ~AbsRWInterface() = 0;

    virtual int write(QByteArray data) = 0;
    virtual int waitForWrite(QByteArray data, int msecs = -1) = 0;
    virtual int read(QByteArray &data) = 0;
    virtual int waitForRead(QByteArray &data, int msecs = -1) = 0;

};

#endif // ABSRWINTERFACE_H

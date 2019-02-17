#ifndef TRANSOBJECT_H
#define TRANSOBJECT_H

#include "commonWigets/addrSetDialog.h"
#include "common/deviceTransceiver.h"

class TransObject
{
public:
    TransObject();
    explicit TransObject(deviceTransceiver *devTrans);
    virtual ~TransObject();
    int setDevTrans(deviceTransceiver *devTrans);

protected:
    int m_devAddr;
    deviceTransceiver *m_pDevTranrecv;
};

#endif // TRANSOBJECT_H

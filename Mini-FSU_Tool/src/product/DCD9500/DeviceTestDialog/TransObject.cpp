#include "TransObject.h"

TransObject::TransObject()
{
    m_devAddr = addrSetDialog::currentDevAddr();
}

TransObject::TransObject(deviceTransceiver *devTrans) :
    TransObject()
{
    this->setDevTrans(devTrans);
}

TransObject::~TransObject()
{

}

int TransObject::setDevTrans(deviceTransceiver *devTrans)
{
    m_pDevTranrecv = devTrans;

    return 0;
}

#include "absSetDialog.h"

absSetDialog::absSetDialog()
{

}

absSetDialog::~absSetDialog()
{
    m_PMDevType = 0;
    m_PMVer = 0;
}

int absSetDialog::setPMProperty(int devType, int ver)
{
    m_PMDevType = devType;
    m_PMVer = ver;

    return 0;
}

void absSetDialog::memberfromMaindow(deviceTransceiver *dev, waittingDialog *dialog, QByteArray *byte)
{
    m_devTranrecv = dev;
    m_wDialog = dialog;
    m_serialRevCache = byte;
}

void absSetDialog::memberfromMaindow(deviceTransceiver *dev)
{
    m_devTranrecv = dev;
}

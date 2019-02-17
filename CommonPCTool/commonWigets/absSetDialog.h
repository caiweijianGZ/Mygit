#ifndef ABSSETDIALOG_H
#define ABSSETDIALOG_H

#include <QObject>
#include "waittingDialog.h"
#include "common/deviceTransceiver.h"
#include "PMBusProtocol/PMBusProtocol.h"

class absSetDialog
{
public:
    virtual ~absSetDialog();
    int setPMProperty(int devType, int ver);
    void memberfromMaindow(deviceTransceiver *dev, waittingDialog *dialog, QByteArray *byte);
    void memberfromMaindow(deviceTransceiver *dev);

protected:
    absSetDialog();

protected:
    deviceTransceiver *m_devTranrecv;
    waittingDialog *m_wDialog;
    QByteArray *m_serialRevCache;
    int m_PMDevType;
    int m_PMVer;
    int m_TP02DevType;
    int m_TP02Ver;
};

#endif // ABSSETDIALOG_H

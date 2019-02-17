#ifndef DCD9500_MINIFSUTOOL_H
#define DCD9500_MINIFSUTOOL_H

#include <QTabWidget>
#include "MiniFSUTool.h"
#include "ChannelSetDialog/ChannelSetDialog.h"
#include "DeviceTestDialog/AITestDialog.h"
#include "DeviceTestDialog/DITestDialog.h"
#include "DeviceTestDialog/NBIotTestDialog.h"
#include "DeviceTestDialog/UartTestDialog.h"
#include "DeviceDetailDialog.h"
#include "OtherSetDialog.h"
#include "EleDetailDialog.h"
#include "SerialSetDialog.h"
#include "CalArgsSetDialog.h"
#include "BettaryInfoDialog.h"
#include "WorkStatusDialog.h"
#include "TimeSetDialog.h"
#include "DeviceResetDialog.h"
#include "ConnectStatesDialog.h"

#define DCD9500_APP_NAME        "Mini-FSU管理程序_beta"
#define DCD9500_APP_VERSON      "V1.2.2"

class DCD9500_MiniFSUTool : public MiniFSUTool
{
    Q_OBJECT
public:
    DCD9500_MiniFSUTool();

protected:
    virtual int funcInit();
    virtual int menuInit();

protected:
    void actionAbout();
};

#endif // DCD9500_MINIFSUTOOL_H

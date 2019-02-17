#ifndef UPGRADEDIALOG_TYPE_H
#define UPGRADEDIALOG_TYPE_H

#include <stdint.h>
#include <string.h>
#include <QMap>
#include "TP02Protocol/TP02Protocol.h"

const QMap<uint8_t, QString> mapDeviceTypeName =
{
    {TP02_DEVICE_TYPE_BDM,      "BMD8150"},
    {TP02_DEVICE_TYPE_WIRE_BDM, "BMD-W-M"},
    {TP02_DEVICE_TYPE_AC_METER, "JLDB-W-102"},
    {TP02_DEVICE_TYPE_DC_METER, "ZLDB-W-102"},
    {TP02_DEVICE_TYPE_ACD,      "KT-W-102"},
    {TP02_DEVICE_TYPE_ACD_8210, "ACD8210"},
    {TP02_DEVICE_TYPE_DCD9500,  "DCD9500"},
    {TP02_DEVICE_TYPE_YIKE_POWER, "YIKE_POWER"},
};


#endif // UPGRADEDIALOG_TYPE_H

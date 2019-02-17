#ifndef CHANNEL_TYPE_H
#define CHANNEL_TYPE_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#define CHANNEL_ENABLE          1
#define CHANNEL_DISABLE         0

#define SENSOR_TYPE_VOL         0
#define SENSOR_TYPE_CURRENT     1
#define SENSOR_TYPE_DIGITAL     2
#define SENSOR_TYPE_OPEN        3
#define SENSOR_TYPE_CLOSE       4

#define SIGNAL_TYPE_YX          0
#define SIGNAL_TYPE_YC          1
#define SIGNAL_TYPE_YK          2
#define SIGNAL_TYPE_YT          3


#define CHN_JSON_VALUE_ACTIVE       "active"
#define CHN_JSON_VALUE_DEVCODE      "devCode"
#define CHN_JSON_VALUE_ID           "id"
#define CHN_JSON_VALUE_SIGNALIDS    "signalIds"
#define CHN_JSON_VALUE_TAG          "tag"

#define CHN_JSON_VALUE_SENSOR_TYPE  "sensorType"
#define CHN_DI_JSON_VALUE_MAX_VALUE "maxValue"
#define CHN_DI_JSON_VALUE_MIN_VALUE "minValue"

#define CHN_AI_DI_JSON_VALUE_ARRAY  "AI_DIChannels"
#define CHN_DO_JSON_VALUE_ARRAY     "DOChannels"

#define DICT_JSON_VALUE_DEVICES     "Devices"
#define DICT_JSON_VALUE_TYPE        "Type"
#define DICT_JSON_VALUE_NAME        "Name"
#define DICT_JSON_VALUE_SIGNALS     "Signals"
#define DICT_JSON_VALUE_ID          "ID"
#define DICT_JSON_VALUE_SIGNAME     "SigName"
#define DICT_JSON_VALUE_ALMMSG      "AlarmMsg"
#define DICT_JSON_VALUE_NORMSG      "NormalMsg"

const QMap<int, QString> mapChannelActive = {
    {CHANNEL_ENABLE, "启用"},
    {CHANNEL_DISABLE, "禁止"},
};

const QMap<int, QString> mapSensorTypes = {
    {SENSOR_TYPE_VOL, "电压"},
    {SENSOR_TYPE_CURRENT, "电流"},
    {SENSOR_TYPE_DIGITAL, "数字"},
    {SENSOR_TYPE_OPEN, "常开"},
    {SENSOR_TYPE_CLOSE, "常闭"},
};

const QMap<int, QString> mapDISensorTypes = {
    {SENSOR_TYPE_OPEN, "常开"},
    {SENSOR_TYPE_CLOSE, "常闭"},
};

const QMap<int, QString> mapChnSigTypes = {
    {SIGNAL_TYPE_YC, "遥测"},
    {SIGNAL_TYPE_YK, "遥控"},
    {SIGNAL_TYPE_YT, "遥调"},
    {SIGNAL_TYPE_YX, "遥信"},
};



#endif // CHANNEL_TYPE_H

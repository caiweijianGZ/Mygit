#ifndef SERIALITEM_TYPE_H
#define SERIALITEM_TYPE_H

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#define SERIAL_JSON_VALUE_SERIALS          "serials"
#define SERIAL_JSON_VALUE_ACTIVE           "active"
#define SERIAL_JSON_VALUE_ID               "id"
#define SERIAL_JSON_VALUE_DEVICES          "devices"
#define SERIAL_JSON_VALUE_DEVCODE          "devCode"
#define SERIAL_JSON_VALUE_ADDR485          "addr485"
#define SERIAL_JSON_VALUE_LIBNAME          "libName"

#define SERIAL_ENABLE          1
#define SERIAL_DISABLE         0

const QMap<int, QString> mapSerialActive = {
    {SERIAL_ENABLE, "启用"},
    {SERIAL_DISABLE, "禁止"},
};

typedef struct bus485Device
{
    uint8_t addr485;
    QString devCode;
}bus485Device;

#endif // SERIALITEM_TYPE_H

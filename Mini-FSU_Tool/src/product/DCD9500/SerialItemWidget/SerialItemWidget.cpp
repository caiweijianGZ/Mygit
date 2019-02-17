#include "SerialItemWidget.h"

SerialItemWidget::SerialItemWidget(deviceTransceiver *devTrans, const QJsonObject &obj, QWidget *parent) :
    QWidget(parent),
    TransObject(devTrans)
{
    id = 0;
    active.addItems(QStringList()
                    << mapSerialActive.value(SERIAL_ENABLE)
                    <<  mapSerialActive.value(SERIAL_DISABLE));

    libBtn.setText("导入");
    connect(&libBtn, SIGNAL(clicked(bool)), this, SLOT(libImportSlot()));

    if ( !obj.isEmpty() )
    {
        this->JsonObj2SelItemWidget(obj);
    }
}
int SerialItemWidget::JsonObj2SelItemWidget(const QJsonObject &obj)
{
    QJsonArray deviceArray = obj.value(SERIAL_JSON_VALUE_DEVICES).toArray();
    for (int i=0; i<deviceArray.count(); i++)
    {
        QString str_devcode = deviceArray.at(i).toObject().value(SERIAL_JSON_VALUE_DEVCODE).toString();
        uint8_t addr485 = (uint8_t)deviceArray.at(i).toObject().value(SERIAL_JSON_VALUE_ADDR485).toInt();
        bus485Device device;
        device.addr485 = addr485;
        device.devCode = str_devcode;
        deviceList.append(device);
    }

    this->active.setCurrentText(mapSerialActive.value(obj.value(SERIAL_JSON_VALUE_ACTIVE).toInt()));
    this->libName.setText(obj.value(SERIAL_JSON_VALUE_LIBNAME).toString());
    this->id = obj.value(SERIAL_JSON_VALUE_ID).toInt();
    this->devCode.setText(deviceList.first().devCode);

    return 0;
}

QJsonObject SerialItemWidget::toJsonObject(bool *ok)
{
    QJsonObject serialObj, deviceObj;
    QJsonArray deviceArray;

    if ( this->devCode.text().length() > 14 )
    {
        goto ERR;
    }

    deviceObj.insert(SERIAL_JSON_VALUE_DEVCODE, this->devCode.text());
    deviceObj.insert(SERIAL_JSON_VALUE_ADDR485, 1);
    deviceArray.append(deviceObj);

    serialObj.insert(SERIAL_JSON_VALUE_ACTIVE, mapSerialActive.key(this->active.currentText()));
    serialObj.insert(SERIAL_JSON_VALUE_LIBNAME, this->libName.text());
    serialObj.insert(SERIAL_JSON_VALUE_DEVICES, deviceArray);
    serialObj.insert(SERIAL_JSON_VALUE_ID, this->id);

    *ok = true;
    return serialObj;

ERR:
    *ok = false;
    return serialObj;

}

void SerialItemWidget::setEnabled(bool enable)
{
    this->active.setEnabled(enable);
    this->devCode.setEnabled(enable);
    this->libName.setEnabled(enable);
    this->libBtn.setEnabled(enable);
    QWidget::setEnabled(enable);
}

void SerialItemWidget::libImportSlot()
{
    LibImportDialog lDialog(m_pDevTranrecv);
    lDialog.setImportComIndex(this->id);
    lDialog.exec();
}

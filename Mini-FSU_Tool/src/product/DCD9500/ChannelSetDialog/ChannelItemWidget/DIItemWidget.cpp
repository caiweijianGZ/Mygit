#include "DIItemWidget.h"

DIItemWidget::DIItemWidget(QWidget *parent)
    : DIItemWidget(QJsonObject(), parent)
{

}

DIItemWidget::DIItemWidget(const QJsonObject &obj, QWidget *parent)
    : ChnItemWidget(parent)
{
    sensorType.addItems(QStringList()
                        << mapSensorTypes.value(SENSOR_TYPE_OPEN)
                        << mapSensorTypes.value(SENSOR_TYPE_CLOSE));

    if ( !obj.isEmpty() )
    {
        this->JsonObj2ChnItemWidget( obj);
    }
}

int DIItemWidget::JsonObj2ChnItemWidget(const QJsonObject &obj)
{
    DIItemWidget *itemWidget = this;

    itemWidget->id = obj.value(CHN_JSON_VALUE_ID).toInt();
    itemWidget->active.setCurrentText(mapChannelActive.value(obj.value(CHN_JSON_VALUE_ACTIVE).toInt()));
    itemWidget->tag.setText(obj.value(CHN_JSON_VALUE_TAG).toString());
    itemWidget->devCode.setText(obj.value(CHN_JSON_VALUE_DEVCODE).toString());//to uint64??
    itemWidget->sensorType.setCurrentText(mapSensorTypes.value(obj.value(CHN_JSON_VALUE_SENSOR_TYPE).toInt()));

    QJsonArray signalArry = obj.value(CHN_JSON_VALUE_SIGNALIDS).toArray();
    for ( int i=0; i<signalArry.count(); i++ )
    {
        //itemWidget->signalIdList.append(signalArry.at(i).toString().toULongLong());//to uint64??
        itemWidget->signalIdList.append(signalArry.at(i).toInt());//to uint64??
    }

    return 0;
}

QJsonObject DIItemWidget::toJsonObject(bool *ok)
{
    QJsonObject DIObj;
    QJsonArray signalArry;
    uint64_t devCode = 0;
    bool isCoverOk = false;
    DIItemWidget *itemWidget = this;

    if ( itemWidget->devCode.text().length() > 14 )
    {
        *ok = false;
        return DIObj;
    }

    devCode = itemWidget->devCode.text().toULongLong(&isCoverOk);
    if ( !isCoverOk )
    {
        *ok = isCoverOk;
        return DIObj;
    }

    if ( itemWidget->tag.text().length() > 10 )
    {
        *ok = false;
        return DIObj;
    }

    for ( int i=0; i<itemWidget->signalIdList.count(); i++ )
    {
        signalArry.append((int)itemWidget->signalIdList.at(i));
    }

    DIObj.insert(CHN_JSON_VALUE_ID, itemWidget->id);
    DIObj.insert(CHN_JSON_VALUE_ACTIVE, mapChannelActive.key(itemWidget->active.currentText()));
    DIObj.insert(CHN_JSON_VALUE_TAG, itemWidget->tag.text());
    DIObj.insert(CHN_JSON_VALUE_DEVCODE, QString::number(devCode));
    DIObj.insert(CHN_JSON_VALUE_SIGNALIDS, signalArry);
    DIObj.insert(CHN_JSON_VALUE_SENSOR_TYPE, mapSensorTypes.key(itemWidget->sensorType.currentText()));

    *ok = isCoverOk;
    return DIObj;
}

void DIItemWidget::setEnabled(bool enable)
{
    ChnItemWidget::setEnabled(enable);

    sensorType.setEnabled(enable);
}


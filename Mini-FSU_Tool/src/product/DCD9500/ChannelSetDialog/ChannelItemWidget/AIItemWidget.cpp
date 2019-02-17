#include "AIItemWidget.h"

AIItemWidget::AIItemWidget(QWidget *parent)
    : AIItemWidget(QJsonObject(), parent)
{

}

AIItemWidget::AIItemWidget(const QJsonObject &obj, QWidget *parent)
    : ChnItemWidget(parent)
{
    sensorType.addItems(QStringList()
                        //<< mapSensorTypes.value(SENSOR_TYPE_DIGITAL)
                        << mapSensorTypes.value(SENSOR_TYPE_VOL)
                        << mapSensorTypes.value(SENSOR_TYPE_CURRENT));
    if ( !obj.isEmpty() )
    {
        this->JsonObj2ChnItemWidget(obj);
    }
}

int AIItemWidget::JsonObj2ChnItemWidget(const QJsonObject &obj)
{
    AIItemWidget *itemWidget = this;

    itemWidget->id = obj.value(CHN_JSON_VALUE_ID).toInt();
    itemWidget->active.setCurrentText(mapChannelActive.value(obj.value(CHN_JSON_VALUE_ACTIVE).toInt()));
    itemWidget->tag.setText(obj.value(CHN_JSON_VALUE_TAG).toString());
    itemWidget->devCode.setText(obj.value(CHN_JSON_VALUE_DEVCODE).toString());//to uint64??
    itemWidget->sensorType.setCurrentText(mapSensorTypes.value(obj.value(CHN_JSON_VALUE_SENSOR_TYPE).toInt()));
    itemWidget->maxValue.setText(QString::number(obj.value(CHN_DI_JSON_VALUE_MAX_VALUE).toInt()));
    itemWidget->minValue.setText(QString::number(obj.value(CHN_DI_JSON_VALUE_MIN_VALUE).toInt()));

    QJsonArray signalArry = obj.value(CHN_JSON_VALUE_SIGNALIDS).toArray();
    for ( int i=0; i<signalArry.count(); i++ )
    {
        //itemWidget->signalIdList.append(signalArry.at(i).toString().toULongLong());//to uint64??
        itemWidget->signalIdList.append(signalArry.at(i).toInt());//to uint64??
    }

    return 0;
}

QJsonObject AIItemWidget::toJsonObject(bool *ok)
{
    QJsonObject AIObj;
    QJsonArray signalArry;
    uint64_t devCode = 0;
    int maxValue, minValue = 0;
    bool isCoverOk = false;
    AIItemWidget *itemWidget = this;

    if ( itemWidget->devCode.text().length() > 14 )
    {
        *ok = false;
        return AIObj;
    }

    devCode = itemWidget->devCode.text().toULongLong(&isCoverOk);
    if ( !isCoverOk )
    {
        *ok = isCoverOk;
        return AIObj;
    }

    if ( itemWidget->tag.text().length() > 10 )
    {
        *ok = false;
        return AIObj;
    }

    maxValue = itemWidget->maxValue.text().toInt(&isCoverOk);
    if ( !isCoverOk )
    {
        *ok = isCoverOk;
        return AIObj;
    }

    minValue = itemWidget->minValue.text().toInt(&isCoverOk);
    if ( !isCoverOk )
    {
        *ok = isCoverOk;
        return AIObj;
    }

    for ( int i=0; i<itemWidget->signalIdList.count(); i++ )
    {
        signalArry.append((int)itemWidget->signalIdList.at(i));
    }

    AIObj.insert(CHN_JSON_VALUE_ID, itemWidget->id);
    AIObj.insert(CHN_JSON_VALUE_ACTIVE, mapChannelActive.key(itemWidget->active.currentText()));
    AIObj.insert(CHN_JSON_VALUE_TAG, itemWidget->tag.text());
    AIObj.insert(CHN_JSON_VALUE_DEVCODE, QString::number(devCode));
    AIObj.insert(CHN_JSON_VALUE_SIGNALIDS, signalArry);

    AIObj.insert(CHN_JSON_VALUE_SENSOR_TYPE, mapSensorTypes.key(itemWidget->sensorType.currentText()));
    AIObj.insert(CHN_DI_JSON_VALUE_MAX_VALUE, maxValue);
    AIObj.insert(CHN_DI_JSON_VALUE_MIN_VALUE, minValue);

    *ok = isCoverOk;
    return AIObj;
}

void AIItemWidget::setEnabled(bool enable)
{
    ChnItemWidget::setEnabled(enable);

    sensorType.setEnabled(enable);
    maxValue.setEnabled(enable);
    minValue.setEnabled(enable);
}

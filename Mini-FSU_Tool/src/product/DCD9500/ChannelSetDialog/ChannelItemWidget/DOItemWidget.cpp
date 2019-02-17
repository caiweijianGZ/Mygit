#include "DOItemWidget.h"

DOItemWidget::DOItemWidget(QWidget *parent)
    : DOItemWidget(QJsonObject(), parent)
{

}

DOItemWidget::DOItemWidget(const QJsonObject &obj, QWidget *parent)
    : ChnItemWidget(parent)
{
    if ( !obj.isEmpty() )
    {
        this->JsonObj2ChnItemWidget(obj);
    }
}

int DOItemWidget::JsonObj2ChnItemWidget(const QJsonObject &obj)
{
    DOItemWidget *itemWidget = this;

    itemWidget->id = obj.value(CHN_JSON_VALUE_ID).toInt();
    itemWidget->active.setCurrentText(mapChannelActive.value(obj.value(CHN_JSON_VALUE_ACTIVE).toInt()));
    itemWidget->tag.setText(obj.value(CHN_JSON_VALUE_TAG).toString());
    itemWidget->devCode.setText(obj.value(CHN_JSON_VALUE_DEVCODE).toString());//to uint64??

    QJsonArray signalArry = obj.value(CHN_JSON_VALUE_SIGNALIDS).toArray();
    for ( int i=0; i<signalArry.count(); i++ )
    {
        //itemWidget->signalIdList.append(signalArry.at(i).toString().toULongLong());//to uint64??
        itemWidget->signalIdList.append(signalArry.at(i).toInt());//to uint64??
    }

    return 0;
}

QJsonObject DOItemWidget::toJsonObject(bool *ok)
{
    QJsonObject DOObj;
    QJsonArray signalArry;
    uint64_t devCode = 0;
    bool isCoverOk = false;
    DOItemWidget *itemWidget = this;

    if ( itemWidget->devCode.text().length() > 14 )
    {
        *ok = false;
        return DOObj;
    }

    devCode = itemWidget->devCode.text().toULongLong(&isCoverOk);
    if ( !isCoverOk )
    {
        *ok = isCoverOk;
        return DOObj;
    }

    if ( itemWidget->tag.text().length() > 10 )
    {
        *ok = false;
        return DOObj;
    }

    for ( int i=0; i<itemWidget->signalIdList.count(); i++ )
    {
        signalArry.append((int)itemWidget->signalIdList.at(i));
    }

    DOObj.insert(CHN_JSON_VALUE_ID, itemWidget->id);
    DOObj.insert(CHN_JSON_VALUE_ACTIVE, mapChannelActive.key(itemWidget->active.currentText()));
    DOObj.insert(CHN_JSON_VALUE_TAG, itemWidget->tag.text());
    DOObj.insert(CHN_JSON_VALUE_DEVCODE, QString::number(devCode));
    DOObj.insert(CHN_JSON_VALUE_SIGNALIDS, signalArry);

    *ok = isCoverOk;
    return DOObj;
}


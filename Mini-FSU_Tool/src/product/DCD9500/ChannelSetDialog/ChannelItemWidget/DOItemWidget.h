#ifndef DOITEMWIDGET_H
#define DOITEMWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>

#include "ChnItemWidget.h"

class DOItemWidget : public ChnItemWidget
{
    Q_OBJECT

public:
    DOItemWidget(QWidget *parent = 0);
    DOItemWidget(const QJsonObject &obj, QWidget *parent = 0);
    virtual int JsonObj2ChnItemWidget(const QJsonObject &obj);
    virtual QJsonObject toJsonObject(bool *ok = NULL);

};

#endif // DOITEMWIDGET_H

#ifndef DIITEMWIDGET_H
#define DIITEMWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>

#include "ChnItemWidget.h"

class DIItemWidget :  public ChnItemWidget
{
    Q_OBJECT

public:
    DIItemWidget(QWidget *parent = 0);
    DIItemWidget(const QJsonObject &obj, QWidget *parent = 0);
    virtual int JsonObj2ChnItemWidget(const QJsonObject &obj);
    virtual QJsonObject toJsonObject(bool *ok = NULL);
    void setEnabled(bool enable);

public:
    QComboBox sensorType;

};

#endif // DIITEMWIDGET_H

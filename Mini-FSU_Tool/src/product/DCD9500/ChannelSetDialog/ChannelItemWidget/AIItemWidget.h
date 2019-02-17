#ifndef AIITEMWIDGET_H
#define AIITEMWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>

#include "ChnItemWidget.h"

class AIItemWidget : public ChnItemWidget
{
    Q_OBJECT

public:
    explicit AIItemWidget(QWidget *parent = 0);
    AIItemWidget(const QJsonObject &obj, QWidget *parent = 0);
    virtual int JsonObj2ChnItemWidget(const QJsonObject &obj);
    virtual QJsonObject toJsonObject(bool *ok = NULL);
    void setEnabled(bool enable);

public:
    QComboBox sensorType;
    QLineEdit maxValue;
    QLineEdit minValue;
};

#endif // AIITEMWIDGET_H

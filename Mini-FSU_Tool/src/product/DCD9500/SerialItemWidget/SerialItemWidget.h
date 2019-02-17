#ifndef SERIALITEMWIDGET_H
#define SERIALITEMWIDGET_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>

#include "serialitem_type.h"
#include "LibImportDialog/LibImportDialog.h"
#include "product/DCD9500/TransObject.h"


class SerialItemWidget : public QWidget, TransObject
{
    Q_OBJECT

public:
    explicit SerialItemWidget(deviceTransceiver *devTrans, const QJsonObject &obj, QWidget *parent = NULL);
    int JsonObj2SelItemWidget(const QJsonObject &obj);
    QJsonObject toJsonObject(bool *ok = NULL);
    void setEnabled(bool enable);

public:
    QComboBox active;
    QLineEdit devCode;
    QLabel libName;
    QPushButton libBtn;

protected:
    uint8_t id;
    QList<bus485Device> deviceList;

private slots:
    void libImportSlot();

};

#endif // SERIALITEMWIDGET_H

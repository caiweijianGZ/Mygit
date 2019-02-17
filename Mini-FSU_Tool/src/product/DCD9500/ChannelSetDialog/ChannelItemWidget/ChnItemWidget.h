#ifndef CHNITEMWIDGET_H
#define CHNITEMWIDGET_H

#include <QWidget>
#include "SignalIdsEditDialog.h"
#include "../channel_type.h"

#define SIGNAL_DICT_PATH   "./signal_dic.json"

class ChnItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChnItemWidget(QWidget *parent = 0);
    virtual ~ChnItemWidget();
    virtual int JsonObj2ChnItemWidget(const QJsonObject &obj) = 0;
    virtual QJsonObject toJsonObject(bool *ok = NULL) = 0;
    void setEnabled(bool enable);

public:
    QLineEdit tag;
    QComboBox active;
    QLineEdit devCode;
    QPushButton signalIdsBtn;

protected:
    int id;
    QList<uint64_t> signalIdList;

public slots:
    void signalIdsEditSlot();

};

#endif // CHNITEMWIDGET_H

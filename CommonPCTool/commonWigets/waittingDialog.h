#ifndef WAITTINGDIALOG_H
#define WAITTINGDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QMovie>
#include <QTimer>
#include "common/deviceTransceiver.h"
#include "TP02Protocol/TP02Protocol.h"
#include "PMBusProtocol/PMBusProtocol.h"

namespace Ui {
class waittingDialog;
}

class waittingDialog : public QDialog
{
    Q_OBJECT

public:
    enum ProtocolType
    {
        PMbus,
        TP02,
    };
    explicit waittingDialog(QWidget *parent = 0);
    waittingDialog(deviceTransceiver *devTranscviver, ProtocolType protocol, QWidget *parent = 0);
    ~waittingDialog();
    int setProtocolType(waittingDialog::ProtocolType prototolType);
    void setMessage(QString message);
    int waitForClose(int msecs = 5000);
    void acceptAtOnce();

protected:
    virtual void paintEvent(QPaintEvent *);
    int movieStart();

private:
    Ui::waittingDialog *ui;
    QMovie movie;
    QTimer *m_waitTimer;
    ProtocolType m_protocolType;
    QByteArray m_readCache;

private slots:
    void readDataSlot(QByteArray data);

};

#endif // WAITTINGDIALOG_H

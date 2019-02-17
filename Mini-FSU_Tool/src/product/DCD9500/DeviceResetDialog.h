#ifndef DEVICERESETDIALOG_H
#define DEVICERESETDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"

namespace Ui {
class DeviceResetDialog;
}

class DeviceResetDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit DeviceResetDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~DeviceResetDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DeviceResetDialog *ui;
};

#endif // DEVICERESETDIALOG_H

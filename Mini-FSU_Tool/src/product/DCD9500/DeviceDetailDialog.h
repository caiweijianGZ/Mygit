#ifndef DEVICEDETAILDIALOG_H
#define DEVICEDETAILDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"

#pragma pack(1)
typedef struct extraDevinfo
{
    uint32_t romSize;
}extraDevinfo_t;
#pragma pack()

namespace Ui {
class DeviceDetailDialog;
}

class DeviceDetailDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit DeviceDetailDialog(QWidget *parent = 0);
    DeviceDetailDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~DeviceDetailDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DeviceDetailDialog *ui;
};

#endif // DEVICEDETAILDIALOG_H

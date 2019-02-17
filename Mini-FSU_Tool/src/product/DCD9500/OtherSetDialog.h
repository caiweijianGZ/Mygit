#ifndef OTHERSETDIALOG_H
#define OTHERSETDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"

#define POWER_TYPE_UNKNOW               0x00
#define POWER_TYPE_DC_48V               0x01
#define POWER_TYPE_DC_240V              0x02
#define POWER_TYPE_AC_220V              0x03

const QMap<uint8_t, QString> mapPowerTypes = {
    {POWER_TYPE_UNKNOW, "未知"},
    {POWER_TYPE_DC_48V, "直流-48V"},
    {POWER_TYPE_DC_240V, "直流-240V"},
    {POWER_TYPE_AC_220V, "交流-220V"}
};


namespace Ui {
class OtherSetDialog;
}

class OtherSetDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit OtherSetDialog(QWidget *parent = 0);
    OtherSetDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~OtherSetDialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::OtherSetDialog *ui;
};

#endif // OTHERSETDIALOG_H

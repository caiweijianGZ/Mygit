#ifndef TIMESETDIALOG_H
#define TIMESETDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"

#define TIME_FORMAT         "yyyy-MM-dd hh:mm:ss"

namespace Ui {
class TimeSetDialog;
}

class TimeSetDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit TimeSetDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~TimeSetDialog();

protected:
    void TP02SetTime(const QString &time);

private slots:
    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

private:
    Ui::TimeSetDialog *ui;
};

#endif // TIMESETDIALOG_H

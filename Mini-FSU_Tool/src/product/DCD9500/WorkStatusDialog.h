#ifndef WORKSTATUSDIALOG_H
#define WORKSTATUSDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"

namespace Ui {
class WorkStatusDialog;
}

class WorkStatusDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit WorkStatusDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~WorkStatusDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::WorkStatusDialog *ui;
};

#endif // WORKSTATUSDIALOG_H

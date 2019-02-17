#ifndef UARTTESTDIALOG_H
#define UARTTESTDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "TP02_MFSU/TP02_MFSU.h"
#include "TransObject.h"

namespace Ui {
class UartTestDialog;
}

class UartTestDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit UartTestDialog(QWidget *parent = 0);
    UartTestDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~UartTestDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::UartTestDialog *ui;
};

#endif // UARTTESTDIALOG_H

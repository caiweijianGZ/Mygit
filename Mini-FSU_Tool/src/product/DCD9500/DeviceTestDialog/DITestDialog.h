#ifndef DITESTDIALOG_H
#define DITESTDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "TP02_MFSU/TP02_MFSU.h"
#include "TransObject.h"

#define CHN_DI_INDEX_ALL       0xFF

namespace Ui {
class DITestDialog;
}

class DITestDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit DITestDialog(QWidget *parent = 0);
    DITestDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~DITestDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DITestDialog *ui;
};

#endif // DITESTDIALOG_H

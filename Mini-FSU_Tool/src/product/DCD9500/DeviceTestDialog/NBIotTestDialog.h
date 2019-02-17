#ifndef NBIOTTESTDIALOG_H
#define NBIOTTESTDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "TP02_MFSU/TP02_MFSU.h"
#include "TransObject.h"

namespace Ui {
class NBIotTestDialog;
}

class NBIotTestDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit NBIotTestDialog(QWidget *parent = 0);
    NBIotTestDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~NBIotTestDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::NBIotTestDialog *ui;
};

#endif // NBIOTTESTDIALOG_H

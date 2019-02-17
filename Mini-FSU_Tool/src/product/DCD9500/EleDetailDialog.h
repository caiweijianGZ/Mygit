#ifndef ELEDETAILDIALOG_H
#define ELEDETAILDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"

namespace Ui {
class EleDetailDialog;
}

class EleDetailDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit EleDetailDialog(QWidget *parent = 0);
    explicit EleDetailDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~EleDetailDialog();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::EleDetailDialog *ui;
};

#endif // ELEDETAILDIALOG_H

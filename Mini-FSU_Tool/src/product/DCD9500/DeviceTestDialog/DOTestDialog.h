#ifndef DOTESTDIALOG_H
#define DOTESTDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "TP02_MFSU/TP02_MFSU.h"
#include "TransObject.h"

#define CHN_DO_INDEX_ALL       0xFF

namespace Ui {
class DOTestDialog;
}

class DOTestDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    enum DOValue{
        On = 1,
        Off = 0
    };

    explicit DOTestDialog(QWidget *parent = 0);
    DOTestDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~DOTestDialog();

protected:
    int DOControl(int index, DOValue value);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::DOTestDialog *ui;
};

#endif // DOTESTDIALOG_H

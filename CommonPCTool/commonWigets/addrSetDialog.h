#ifndef ADDRSETDIALOG_H
#define ADDRSETDIALOG_H

#include <QDialog>
#include "commonWigets/absSetDialog.h"

namespace Ui {
class addrSetDialog;
}

class addrSetDialog : public QDialog , public absSetDialog
{
    Q_OBJECT

public:
    explicit addrSetDialog(QWidget *parent = 0);
    ~addrSetDialog();
    static int currentDevAddr();

protected:
    static int m_devAddr;

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::addrSetDialog *ui;

};

#endif // ADDRSETDIALOG_H

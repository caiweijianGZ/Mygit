#ifndef AITESTDIALOG_H
#define AITESTDIALOG_H

#include <QDialog>
#include <QMessageBox>

#include "TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"

#define CHN_AI_INDEX_ALL       0xFF

namespace Ui {
class AITestDialog;
}

class AITestDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit AITestDialog(QWidget *parent = 0);
    AITestDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~AITestDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::AITestDialog *ui;
};

#endif // AITESTDIALOG_H

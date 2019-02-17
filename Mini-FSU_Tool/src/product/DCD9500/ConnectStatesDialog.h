#ifndef CONNECTSTATESDIALOG_H
#define CONNECTSTATESDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QIcon>
#include <QPixmap>
#include "TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"

#define NO_PIXMAP_PATH      ":/img/common/no.png"
#define YES_PIXMAP_PATH     ":/img/common/yes.png"

namespace Ui {
class ConnectStatesDialog;
}

class ConnectStatesDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit ConnectStatesDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~ConnectStatesDialog();

protected:
    enum connectState{
        disconnect = 0,
        connect = 1,
    };

    void setModemConState(ConnectStatesDialog::connectState state);
    void setPlatformConState(ConnectStatesDialog::connectState state);

private slots:
    void on_pushButton_clicked();

private:
    Ui::ConnectStatesDialog *ui;
};

#endif // CONNECTSTATESDIALOG_H

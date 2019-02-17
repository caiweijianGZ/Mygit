#ifndef SERIALOPENDIALOG_H
#define SERIALOPENDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "interface/winSerialPortInterface.h"

namespace Ui {
class SerialOpenDialog;
}

class SerialOpenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialOpenDialog(winSerialPortInterface *serialInterface, winSerialInfo_t &sinfo, QWidget *parent = 0);
    ~SerialOpenDialog();
    static bool isSerialOpen();
    int serialOpen();
    int serialClose();

protected:
    int initSerialUi();
    int loadSerialConfigFromUI(winSerialInfo_t &sinfo);
    int serialConfigToDisplay(const winSerialInfo_t &sinfo);

protected:
    static bool m_isSerialOpen;
    winSerialPortInterface *m_pWinSerialInterface;
    winSerialInfo_t &m_serialInfo;

private slots:
    void on_pushButton_clicked();


private:
    Ui::SerialOpenDialog *ui;
};

#endif // SERIALOPENDIALOG_H

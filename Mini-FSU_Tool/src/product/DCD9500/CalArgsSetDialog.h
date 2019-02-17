#ifndef CALARGSSETDIALOG_H
#define CALARGSSETDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QFileDialog>
#include "TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"
#include "commonWigets/addrSetDialog.h"


namespace Ui {
class CalArgsSetDialog;
}

class CalArgsSetDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit CalArgsSetDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~CalArgsSetDialog();

protected:
    int hex2Ascii(unsigned char *asciiData, const unsigned char *hexData, uint16_t hexLen);
    int ascii2Hex(unsigned char *hexData, const unsigned char *asciiData, uint16_t asciiLen);
    int saveData2File(const QString &fileName, const QByteArray &data);

protected:
    int m_devAddr;
    QByteArray m_importFileCache;
    QString m_exportFileName;

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();


private:
    Ui::CalArgsSetDialog *ui;
};

#endif // CALARGSSETDIALOG_H

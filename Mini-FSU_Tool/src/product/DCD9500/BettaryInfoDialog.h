#ifndef BETTARYINFODIALOG_H
#define BETTARYINFODIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"

#define NO_HEAT     0
#define IS_HEAT     1
#define NO_CHARGE   0
#define IS_CHARGE   1

const QMap<uint8_t, QString> mapHeatStatus =
{
    {NO_HEAT, "非加热"},
    {IS_HEAT, "正在加热"},
};

const QMap<uint8_t, QString> mapChargeStatus =
{
    {NO_CHARGE, "非充电"},
    {IS_CHARGE, "正在充电"},
};

namespace Ui {
class BettaryInfoDialog;
}

class BettaryInfoDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit BettaryInfoDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~BettaryInfoDialog();

private slots:
    void on_pushButton_clicked();

private:
    Ui::BettaryInfoDialog *ui;
};

#endif // BETTARYINFODIALOG_H

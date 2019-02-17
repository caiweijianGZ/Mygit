#ifndef SERIALSETDIALOG_H
#define SERIALSETDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QTableWidget>
#include <QMessageBox>

#include "TransObject.h"
#include "SerialItemWidget/SerialItemWidget.h"


namespace Ui {
class SerialSetDialog;
}

class SerialSetDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit SerialSetDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~SerialSetDialog();
    int loadJsonData2Ui(const QByteArray &original);
    int loadJsonData2Ui(const QJsonArray &jsonArry);

protected:
    QByteArray serialConfigCreate() const;
    int saveData2File(const QString &fileName, const QByteArray &data) const;
    int toJsonData(QJsonArray &jsonArry) const;

protected:
    int widgetInit();
    int addSerialItemWidget(QTableWidget *tableWidget, SerialItemWidget *itemWidget);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::SerialSetDialog *ui;

protected:
    QList<SerialItemWidget*> m_serialItemList;

};

#endif // SERIALSETDIALOG_H

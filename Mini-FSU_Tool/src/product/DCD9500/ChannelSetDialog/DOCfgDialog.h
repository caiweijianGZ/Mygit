#ifndef DOCFGDIALOG_H
#define DOCFGDIALOG_H

#include <QDialog>
#include <QJsonArray>
#include <QTableWidget>

#include "ChannelItemWidget/DOItemWidget.h"

namespace Ui {
class DOCfgDialog;
}

class DOCfgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DOCfgDialog(QWidget *parent = 0);
    ~DOCfgDialog();
    int loadJsonData2Ui(const QByteArray &original);
    int loadJsonData2Ui(const QJsonArray &jsonArry);
    int getJsonData(QJsonArray &jsonArry);

protected:
    int addDOItemWidget(QTableWidget *tableWidget, DOItemWidget *itemWidget);

protected:
    QList<DOItemWidget *> m_DOItemList;

private:
    Ui::DOCfgDialog *ui;
};

#endif // DOCFGDIALOG_H

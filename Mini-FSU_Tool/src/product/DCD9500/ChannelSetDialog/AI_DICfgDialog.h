#ifndef AI_DICFGDIALOG_H
#define AI_DICFGDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include "ChannelItemWidget/AIItemWidget.h"
#include "ChannelItemWidget/DIItemWidget.h"

namespace Ui {
class AI_DICfgDialog;
}

class AI_DICfgDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AI_DICfgDialog(QWidget *parent = 0);
    ~AI_DICfgDialog();
    int loadJsonData2Ui(const QByteArray &original);
    int loadJsonData2Ui(const QJsonArray &jsonArry);
    int getJsonData(QJsonArray &jsonArry);

protected:
    int widgetInit();
    int addAIItemWidget(QTableWidget *tableWidget, AIItemWidget *itemWidget);
    int addDIItemWidget(QTableWidget *tableWidget, DIItemWidget *itemWidget);

protected:
    QList<AIItemWidget *> m_AIItemList;
    QList<DIItemWidget *> m_DIItemList;

private slots:
    void ch1FuncChangeslot(int index);
    void ch2FuncChangeslot(int index);

private:
    Ui::AI_DICfgDialog *ui;
};

#endif // AI_DICFGDIALOG_H

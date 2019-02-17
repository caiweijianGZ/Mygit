#ifndef CHANNELSETDIALOG_H
#define CHANNELSETDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QTextCodec>
#include <QDebug>

#include "AI_DICfgDialog.h"
#include "DOCfgDialog.h"
#include "../TransObject.h"
#include "TP02_MFSU/TP02_MFSU.h"
#include "cJSON/cJSON.h"

namespace Ui {
class ChannelSetDialog;
}

class ChannelSetDialog : public QDialog, public TransObject
{
    Q_OBJECT

public:
    explicit ChannelSetDialog(QWidget *parent = 0);
    ChannelSetDialog(deviceTransceiver *devTrans, QWidget *parent = 0);
    ~ChannelSetDialog();
    int addWidget(QString tabName, QWidget *widget);

protected:
    int channelConfigCreateCJson(QByteArray &cfgData);
    int cjsonTest2(QByteArray &cfgData);
    int cjsonParse(QByteArray &cfgData);
    int channelConfigCreate(QByteArray &cfgData);
    int saveData2File(const QString &fileName, const QByteArray &data);
    int gb18030ToUtf8(const QByteArray &gb18030Data, QByteArray &utf8Data);
    int utf8ToGB18030(const QByteArray &utf8Data, QByteArray &gb18030Data);

protected:
    AI_DICfgDialog *m_pAI_DICfgDialog;
    DOCfgDialog *m_pDOCfgDialog;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::ChannelSetDialog *ui;
};

#endif // CHANNELSETDIALOG_H

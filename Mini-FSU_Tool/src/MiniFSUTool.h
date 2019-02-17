#ifndef MINIFSUTOOL_H
#define MINIFSUTOOL_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QDateTime>

#include "PMBusProtocol/PMBusProtocol.h"
#include "common/deviceTransceiver.h"
#include "interface/winSerialPortInterface.h"
#include "commonWigets/waittingDialog.h"
#include "commonWigets/romExportDialog.h"
#include "commonWigets/addrSetDialog.h"
#include "upgrade/upgradeDialog.h"
#include "common/Logger.h"

#include "SerialOpenDialog.h"

#define SERIAL_CONFIG_FILE      "./serial.cfg"
#define LOG_DIRECTORY           "./Logs/"

#define SERIAL_PROTOCOL_TYPE_NONE   0x00
#define SERIAL_PROTOCOL_TYPE_PMBUS  0x01
#define SERIAL_PROTOCOL_TYPE_TP02   0x02

#include "ui_MiniFSUTool.h"

namespace Ui {
class MiniFSUTool;
}

class MiniFSUTool : public QMainWindow
{
    Q_OBJECT

public:
    explicit MiniFSUTool(QWidget *parent = 0);
    ~MiniFSUTool();
    int extraFuncInit();

protected:
    int m_DataInit();
    int loadSerialConfigFile(const QString &fileNamem, winSerialInfo_t &sinfo);
    int saveSerialConfigToFile(const QString &fileName, const winSerialInfo_t &sinfo);
    int devTransceiverInit();
    int absInterfaceAttach(AbsRWInterface *absInterface);
    int absInterfaceDetache(AbsRWInterface *absInterface);

protected:
    //ui
    int widgetInit();
    int setStackedWidgetEnable(bool enable);
    int setSendActionEnable(bool enable);
    int setDevAddr2StatusBar(int);
    int setComStatus2StatusBar(QString str);
    int addWidget(const QIcon &icon, QString name, QWidget *widget);
    int addWidget(QString name, QWidget *widget);
    virtual int funcInit();
    virtual int menuInit();

protected:
    winSerialInfo_t m_serialInfo;
    winSerialPortInterface *m_pWinSerialInterface;
    AbsRWInterface *m_pAbsInterface;
    SerialOpenDialog *m_pSerialDialog;
    deviceTransceiver *m_pDevTranrecv;
    waittingDialog m_wDialog;
    QByteArray m_serialRevCache;
    int m_selProType;
    QList<QAction*> m_actionList;//保存串口操作相关的action，用于批量disable，enable
    int m_devAddr;
    QLabel *m_pAddrStatusLabel;
    QLabel *m_pComStatusLabel;

private slots:

    void serialReadSlot(QByteArray data);
    void interfaceErrorSlot(AbsRWInterface *intface);

    void actionSerialOpen();
    void actionSetAddress();
    void actionExportROM();
    virtual void actionUpgrade();
    virtual void actionAbout();


protected:
    Ui::MiniFSUTool *ui;
};

#endif // MINIFSUTOOL_H

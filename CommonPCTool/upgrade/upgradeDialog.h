#ifndef UPGRADEDIALOG_H
#define UPGRADEDIALOG_H

#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QByteArray>
#include <pthread.h>
#include <unistd.h>
#include <QCloseEvent>
#include "interface/AbsRWInterface.h"
#include "common/Logger.h"
#include "commonWigets/addrSetDialog.h"
#include "upgradedialog_type.h"

#define UPGRADE_ERROR_TYPE_TIMEOUT      -1
#define UPGRADE_ERROR_TYPE_INTERFACE    -1

#define SEND_DATA_LIMIT                 50              //适用于zigbee和spi
#define SEND_PERIOD_LIMIT               (50*1000)      //适用于zigbee和spi
#define SEND_MAX_LEN                    1024    //单次发送最大字节数
#define SEND_ERR_COUNT                  3       //发送没回复最大次数
#define READ_ERR_COUNT                  50      //读串口次数
#define UPGRADE_READ_PERIOD             500     //读间隔ms
#define READ_TOTAL_TIMEOUT              10*1000      //读总超时，单位ms

#define LOGIC_UPGRADE_ERR               -1      //固件升级逻辑，升级错误
#define LOGIC_UPGRADE_FINSH             0       //固件升级逻辑，升级完成
#define LOGIC_UPGRADE_VERSION           1       //固件升级逻辑，升级完成

#define UPGRADE_LOG_DIRECTORY           "./Logs/upgrade/"


namespace Ui {
class upgradeDialog;
}

class upgradeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit upgradeDialog(QWidget *parent = 0);
    ~upgradeDialog();
    int setInterface(AbsRWInterface *pInterface);
    int setUpgDevType(int devType);

protected:
    static void* threadUpgradeV2(void *param);
    int upgradeStart();

    QByteArray packUpgStart();
    QByteArray packUpgEraseFlash();
    QByteArray packUpgFileData(const char *data, uint16_t dataLen, uint16_t offset);
    QByteArray packUpgFinsh();
    QByteArray packGetSoftVer();
    int parseUpgAck(const QByteArray &ack);
    int parseSoftVer(const QByteArray &ack, M3SoftVer_t &softVer);
    int infRead(QByteArray &data);
    int infWrite(QByteArray data);
    int infWriteLimit(QByteArray data, uint16_t sendMaxLen, uint16_t period);//兼容M3上层发送速率,兼容zigbee发送速率
    int infWaitForRead(QByteArray &data, int msecs = -1);
    int infWaitForRead(QByteArray &data, int msecs, int period);

    void setCloseEnable(bool enable);
    int parseFileFormat(QString fileName, M3SoftVer_t &softVer, int &devType);
    int versionCompare(M3SoftVer_t softVer1, M3SoftVer_t softVer2);

signals:
    void dataSend(int bytes);
    void interfaceError(AbsRWInterface *intface);

protected://ui
    void widgetInit();
    void setButtonEnable(bool enable);

protected:
    virtual void closeEvent(QCloseEvent *event);

protected:
    AbsRWInterface *m_interface;
    pthread_t m_tid;
    QByteArray m_fileCache;
    bool m_closeEnable;
    int m_devAddr;
    int m_upgDevType;
    M3SoftVer_t m_loadFileVer;
    Logger *m_pLogger;

private slots:
    void dataSendSlot(int bytes);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::upgradeDialog *ui;

};

#endif // UPGRADEDIALOG_H

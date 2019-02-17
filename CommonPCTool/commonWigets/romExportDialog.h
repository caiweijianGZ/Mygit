#ifndef ROMEXPORTDIALOG_H
#define ROMEXPORTDIALOG_H

#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QByteArray>
#include <pthread.h>
#include <unistd.h>
#include <QCloseEvent>
#include "interface/AbsRWInterface.h"
#include "common/Logger.h"
#include "TP02Protocol/TP02Protocol.h"
#include "commonWigets/addrSetDialog.h"

#define SEND_MAX_LEN                    1024    //单次发送最大字节数
#define READ_MAX_LEN                    512    //单次发送最大字节数
#define SEND_ERR_COUNT                  3       //发送没回复最大次数
#define READ_TOTAL_TIMEOUT              10*1000      //读总超时，单位ms
#define ROM_READ_PERIOD                 1000    //ms

#define LOGIC_EXPORT_START              1       //指令导出逻辑，导出开始
#define LOGIC_EXPORT_DATA               2       //指令导出逻辑，导出中
#define LOGIC_EXPORT_FINSH              3       //指令导出逻辑，导出完成
#define LOGIC_EXPORT_ERR                -1      //指令导出逻辑，导出错误
#define LOGIC_EXPORT_ERR_ARG            -2      //指令导出逻辑，导出错误，导出参数

#define ROM_EXPORT_LOG_DIRECTORY        "./Logs/romExport/"

namespace Ui {
class romExportDialog;
}

class romExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit romExportDialog(QWidget *parent = 0);
    ~romExportDialog();
    int setInterface(AbsRWInterface *pInterface);

protected:
    static void* threadExport(void *param);
    int exportStart();
    QByteArray pkgExportRomAsk(uint32_t offset, uint32_t dataLen);
    QByteArray pkgExportRomData(uint32_t offset, uint32_t dataLen);
    int parseExportRomAck(const QByteArray &ack);
    int parseExportRomData(const QByteArray &ack, QByteArray &data);
    int saveExportData(const QByteArray &data);
    int saveExportCache(const QByteArray &data);
    int saveData2File(const QString &fileName, const QByteArray &data);

    int infWrite(QByteArray data);
    int infRead(QByteArray &data);
    int infWaitForRead(QByteArray &data, int msecs = -1);
    int infWaitForRead(QByteArray &data, int msecs, int period);

    void setCloseEnable(bool enable);

protected:
    void widgetInit();
    void setButtonEnable(bool enable);

protected:
    AbsRWInterface *m_interface;
    pthread_t m_tid;
    QString m_saveFileName;
    bool m_closeEnable;
    uint32_t m_dataOffset;
    uint32_t m_dataLen;
    QByteArray m_romDataCache;
    int m_devAddr;
    Logger *m_pLogger;

private:
    Ui::romExportDialog *ui;

protected:
    virtual void closeEvent(QCloseEvent *event);

signals:
    void dataExport(uint32_t readIndex, int status);
    void interfaceError(AbsRWInterface *intface);


private slots:
    void dataExportSlot(uint32_t readIndex, int status);

private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
};

#endif // ROMEXPORTDIALOG_H

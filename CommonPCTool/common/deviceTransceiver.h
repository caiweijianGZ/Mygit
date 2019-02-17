#ifndef DEVICETRANSCEIVER_H
#define DEVICETRANSCEIVER_H

#include <QObject>
#include <QList>
#include <QDebug>
#include <QThread>
#include <unistd.h>
#include <pthread.h>

#include "interface/AbsRWInterface.h"
#include "RingBuffer.h"

/**
 * @brief The deviceTransceiver class
 * @描述： 通用接口收发器，用于定时发送指令数据，以及接受数据，支持设置全双工和半双工，
 * @函数功能：
 * setRWInterface：设置收发接口
 * setSendPeriod：设置发送间隔
 * setCmdPeriod：设置命令发送间隔（cmd为收发器预置的命令队列，在没有数据需要发送时自动发送cmd数据）
 * setMode：设置收发器的工作模式
 * setCmdList：设置收发器的预置发送队列
 * appendSend：把需要立即发送的数据压如队列，发送优先级高于cmd
 * transferStart：开启收发器
 * transferStop：暂停收发器
 * dataSended：已发送数据的信号
 * readyRead：读取数据的信号
 * 版本2.0，通用接口全为阻塞方式，优化部分函数，提高效率
 */

class deviceTransceiver : public QObject
{
    Q_OBJECT
public:
    enum deviceMode {
        fullDuplex,//全双工模式
        halfDuplex,//半双工模式
    };

public:
    explicit deviceTransceiver(QObject *parent = 0);
    virtual ~deviceTransceiver();    
    int setRWInterface(AbsRWInterface *intface);
    int setSendPeriod(int msecs);
    int setCmdPeriod(int msecs);
    int setMode(deviceMode mode);
    int setCmdList(const QList<QByteArray> &list, bool loop);
    int appendSend(QByteArray byte);
    int recvCacheClear();
    int read(QByteArray &byte, uint32_t len);
    int readAll(QByteArray &byte);
    int transferStart();
    int transferStop();

protected:
    static void* threadDeviceRead(void *param);
    static void* threadDeviceSend(void *param);
    int pushSendList(QByteArray cmd);
    QByteArray popSendList();
    QByteArray popCmdList();

    bool getInfStatus();
    int setInfStatus(bool alive);
    int infSend(QByteArray data);
    int infRead(QByteArray &data);
    int infWaitForSend(QByteArray data, int msecs = -1);
    int infWaitForRead(QByteArray &data, int msecs = -1);

    int threadCreate();
    int waitForResume();

    virtual bool isReadFinsh();


protected:
    AbsRWInterface *m_interface;
    unsigned int m_sendPeriod;
    unsigned int m_cmdPeriod;
    QList<QByteArray> m_cmdList;
    QList<QByteArray> m_sendList;
    int m_cmdIndex;
    pthread_mutex_t m_sendMutex;
    pthread_mutex_t m_cmdMutex;
    pthread_mutex_t m_interfaceMutex;
    pthread_mutex_t m_threadStatusMutex;
    pthread_mutex_t m_interfaceStatusMutex;
    pthread_cond_t m_cond;
    pthread_t m_tid;
    deviceMode m_mode;
    bool m_isThreadRunning;
    bool m_isTransferStart;
    bool m_isCmdLoop;
    bool m_isInterfaceAlive;
    RingBuffer m_ringBuffer;

signals:
    void dataSended(QByteArray data);
    void readyRead(QByteArray data);
    void interfaceError(AbsRWInterface *intface);


};

#endif // DEVICETRANSCEIVER_H

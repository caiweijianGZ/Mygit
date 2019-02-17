#include "common/deviceTransceiver.h"

deviceTransceiver::deviceTransceiver(QObject *parent) :
    QObject(parent)
{
    m_sendPeriod = 0;
    m_cmdIndex = 0;
    m_isInterfaceAlive = false;
    m_isTransferStart = false;
    m_mode = halfDuplex;
    pthread_mutex_init(&m_sendMutex, NULL);
    pthread_mutex_init(&m_cmdMutex, NULL);
    pthread_mutex_init(&m_interfaceMutex, NULL);
    pthread_mutex_init(&m_threadStatusMutex, NULL);
    pthread_mutex_init(&m_interfaceStatusMutex, NULL);
    pthread_cond_init(&m_cond, NULL);
}

deviceTransceiver::~deviceTransceiver()
{

}

int deviceTransceiver::setRWInterface(AbsRWInterface *interface)
{
    pthread_mutex_lock(&m_interfaceMutex);

    m_interface = interface;

    pthread_mutex_unlock(&m_interfaceMutex);

    return 0;
}

int deviceTransceiver::setSendPeriod(int msecs)
{
    m_sendPeriod = msecs;

    return 0;
}

int deviceTransceiver::setCmdPeriod(int msecs)
{
    m_cmdPeriod = msecs;

    return 0;
}

int deviceTransceiver::setMode(deviceMode mode)
{
    m_mode = mode;
    return 0;
}

int deviceTransceiver::setCmdList(const QList<QByteArray> &list, bool loop)
{
    pthread_mutex_lock(&m_cmdMutex);

    m_cmdList = list;
    m_isCmdLoop = loop;

    pthread_mutex_unlock(&m_cmdMutex);

    return 0;
}

int deviceTransceiver::appendSend(QByteArray byte)
{
    if ( !this->getInfStatus() )
    {
        return -1;
    }

    m_sendList.append(byte);

    return byte.length();
}

int deviceTransceiver::recvCacheClear()
{
    m_ringBuffer.clear();

    return 0;
}

int deviceTransceiver::read(QByteArray &byte, uint32_t len)
{
    if ( !this->getInfStatus() )
    {
        return -1;
    }

    unsigned char readBuff[len];
    uint32_t ret = m_ringBuffer.get(readBuff, len);
    byte.append((char*)readBuff, ret);

    return ret;
}

int deviceTransceiver::readAll(QByteArray &byte)
{
    return this->read(byte, m_ringBuffer.getSize());
}

int deviceTransceiver::transferStart()
{
    if ( true == m_isTransferStart )
    {
        return 0;
    }

    static bool isThreadCreated = false;
    if ( isThreadCreated )
    {
        pthread_mutex_lock(&m_threadStatusMutex);
        m_isTransferStart = true;
        pthread_cond_signal(&m_cond);//需要发送两次，分别发送给发和收线程
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_threadStatusMutex);

        return 0;
    }

    if ( 0 == this->threadCreate() )
    {
        isThreadCreated = true;
        usleep(100*1000);
        pthread_mutex_lock(&m_threadStatusMutex);
        m_isTransferStart = true;
        pthread_cond_signal(&m_cond);
        pthread_cond_signal(&m_cond);
        pthread_mutex_unlock(&m_threadStatusMutex);

        return 0;
    }

    return -1;
}

int deviceTransceiver::transferStop()
{
    if ( false == m_isTransferStart )
    {
        return 0;
    }

    pthread_mutex_lock(&m_threadStatusMutex);
    m_isTransferStart = false;
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_threadStatusMutex);

    return 0;
}

void* deviceTransceiver::threadDeviceRead(void *param)
{
    pthread_detach(pthread_self());

    deviceTransceiver * THIS = (deviceTransceiver*)param;
    while (1)
    {
        THIS->m_isThreadRunning = true;
        THIS->waitForResume();

        QByteArray byt_read;      
        int ret = 0;
        if ( THIS->m_mode == halfDuplex )//半双工模式下，阻塞读会导致不能写
        {
            ret = THIS->infWaitForRead(byt_read, 200);
            usleep(50*1000);
            //qDebug() << "read timeout:" << byt_read.toHex();
        }
        else
        {
            //ret = THIS->infRead(byt_read);
            ret = THIS->infWaitForRead(byt_read, 200);
            usleep(50*1000);
        }

        THIS->setInfStatus(true);
        if ( -1 == ret )
        {
            THIS->setInfStatus(false);
            THIS->interfaceError(THIS->m_interface);
            usleep(500*1000);
            continue;
        }

        if ( byt_read.length() )
        {
            THIS->readyRead(byt_read);
            THIS->m_ringBuffer.put((unsigned char*)byt_read.data(), byt_read.length());
        }

        //qDebug() << "read data:" << byt_read << byt_read.length();
    }

    pthread_exit(NULL);
}

void* deviceTransceiver::threadDeviceSend(void *param)
{
    pthread_detach(pthread_self());

    deviceTransceiver * THIS = (deviceTransceiver*)param;

    while (1)
    {
        THIS->m_isThreadRunning = true;
        THIS->waitForResume();

        QByteArray byt_toSend = THIS->popSendList();
        if ( !byt_toSend.isEmpty() )
        {
            int ret = THIS->infSend(byt_toSend);
            THIS->setInfStatus(true);
            if ( -1 == ret )
            {
                THIS->setInfStatus(false);
                THIS->interfaceError(THIS->m_interface);                
                usleep(500*1000);
                continue;
            }
            THIS->dataSended(byt_toSend);
            goto CONTINUE;
        }

        byt_toSend = THIS->popCmdList();
        if ( !byt_toSend.isEmpty() )
        {
            int ret = THIS->infSend(byt_toSend);
            THIS->setInfStatus(true);
            if ( -1 == ret )
            {
                THIS->setInfStatus(false);
                THIS->interfaceError(THIS->m_interface);
                usleep(500*1000);
                continue;
            }
            THIS->dataSended(byt_toSend);
            //QThread::msleep(THIS->m_cmdPeriod);

            goto CONTINUE;
        }

CONTINUE:
        QThread::msleep(THIS->m_sendPeriod);
        //usleep(20*1000);//用于检测发送队列的休眠
        continue;
    }

    pthread_exit(NULL);
}

int deviceTransceiver::pushSendList(QByteArray cmd)
{
    pthread_mutex_lock(&m_sendMutex);

    m_sendList.append(cmd);

    pthread_mutex_unlock(&m_sendMutex);

    return 0;
}

QByteArray deviceTransceiver::popSendList()
{
    QByteArray str_send;

    pthread_mutex_lock(&m_sendMutex);

    if ( !m_sendList.isEmpty() )
    {
        str_send = m_sendList.takeFirst();
    }

    pthread_mutex_unlock(&m_sendMutex);

    return str_send;
}

QByteArray deviceTransceiver::popCmdList()
{
    QByteArray str_cmd;

    pthread_mutex_lock(&m_cmdMutex);

    if ( !m_cmdList.isEmpty() )
    {

        if ( m_isCmdLoop )
        {
            str_cmd = m_cmdList.at(m_cmdIndex);
            if ( (++m_cmdIndex) >= m_cmdList.count() )
            {
                m_cmdIndex = 0;
            }
        }
        else
        {
            str_cmd = m_cmdList.takeFirst();
        }

    }

    pthread_mutex_unlock(&m_cmdMutex);

    return str_cmd;
}

bool deviceTransceiver::getInfStatus()
{
    bool status;
    pthread_mutex_lock(&m_interfaceStatusMutex);

    status = m_isInterfaceAlive;

    pthread_mutex_unlock(&m_interfaceStatusMutex);

    return status;
}

int deviceTransceiver::setInfStatus(bool alive)
{
    pthread_mutex_lock(&m_interfaceStatusMutex);

    m_isInterfaceAlive = alive;

    pthread_mutex_unlock(&m_interfaceStatusMutex);

    return 0;
}

int deviceTransceiver::infSend(QByteArray data)
{
    int n_send;
    if ( m_mode == halfDuplex )
    {
        pthread_mutex_lock(&m_interfaceMutex);

        n_send = m_interface->write(data);

        pthread_mutex_unlock(&m_interfaceMutex);

        return n_send;
    }

    n_send = m_interface->write(data);

    return n_send;
}

int deviceTransceiver::infRead(QByteArray &data)
{
    int ret = 0;
    if ( m_mode == halfDuplex )
    {
        pthread_mutex_lock(&m_interfaceMutex);

        ret = m_interface->read(data);

        pthread_mutex_unlock(&m_interfaceMutex);

        return ret;
    }

    ret = m_interface->read(data);

    return ret;
}

int deviceTransceiver::infWaitForSend(QByteArray data, int msecs)
{
    int n_send;
    if ( m_mode == halfDuplex )
    {
        pthread_mutex_lock(&m_interfaceMutex);

        n_send = m_interface->waitForWrite(data, msecs);

        pthread_mutex_unlock(&m_interfaceMutex);

        return n_send;
    }

    n_send = m_interface->waitForWrite(data, msecs);

    return n_send;
}

int deviceTransceiver::infWaitForRead(QByteArray &data, int msecs)
{
    int ret = 0;
    if ( m_mode == halfDuplex )
    {
        pthread_mutex_lock(&m_interfaceMutex);

        ret = m_interface->waitForRead(data, msecs);

        pthread_mutex_unlock(&m_interfaceMutex);

        return ret;
    }

    ret = m_interface->waitForRead(data, msecs);

    return ret;
}

int deviceTransceiver::threadCreate()
{
    m_isThreadRunning = false;
    int ret = pthread_create(&m_tid, NULL, threadDeviceSend, this);
    if ( 0 != ret )
    {
        perror("pthread_create");
        return -1;
    }
    while ( false == m_isThreadRunning )
    {
        usleep(100*1000);
    }

    m_isThreadRunning = false;
    ret = pthread_create(&m_tid, NULL, threadDeviceRead, this);
    if ( 0 != ret )
    {
        perror("pthread_create");
        return -1;
    }
    while ( false == m_isThreadRunning )//等待线程跑起来
    {
        usleep(100*1000);
    }

    return 0;
}

int deviceTransceiver::waitForResume()
{
    pthread_mutex_lock(&m_threadStatusMutex);
    while ( !m_isTransferStart)
    {
        pthread_cond_wait(&m_cond,&m_threadStatusMutex);
    }
    pthread_mutex_unlock(&m_threadStatusMutex);

    return 0;
}

bool deviceTransceiver::isReadFinsh()
{
    return true;
}


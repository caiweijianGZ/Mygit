#include "Logger.h"

QMap<QString, Logger*> Logger::m_loggerMap;

Logger *Logger::LoggerCreate(const QString &fileName)
{
    QFileInfo qFileInfo(fileName);
    QString absFilePath = qFileInfo.absoluteFilePath();
    if ( absFilePath.isEmpty() )
    {
        return NULL;
    }

    if ( m_loggerMap.contains(absFilePath) )
    {
        return m_loggerMap.value(absFilePath);
    }

    if ( !fileCreate(absFilePath) )
    {
        return NULL;
    }

    return new Logger(absFilePath);
}

Logger::Logger(const QString &fileName)
{
    pthread_mutex_init(&m_writeMutex, NULL);
    pthread_mutex_init(&m_readMutex, NULL);
    m_fileName = fileName;
    m_loggerMap.insert(fileName, this);
    m_logFile.setFileName(fileName);
    m_logFile.open(QIODevice::Append | QIODevice::Text);//独占日志文件，进程关闭后释放
}

bool Logger::fileCreate(const QString &fileName)
{
    QFile file(fileName);
    if ( file.exists() )
    {
        return true;
    }

    QFileInfo qFileInfo(fileName);
    QDir dir(qFileInfo.absolutePath());
    if ( !dir.exists() )
    {
        if ( !dir.mkpath(qFileInfo.absolutePath()) )
        {
            return false;
        }
    }

    if ( !file.open(QIODevice::Append) )
    {
        return false;
    }

    file.close();
    return true;
}

Logger::~Logger()
{
    m_logFile.close();
    m_loggerMap.remove(m_fileName);
}

int Logger::write(const char *str)
{
    return this->write(QString::fromUtf8(str));
}

int Logger::writeWithDate(const char *str)
{
    return this->write(str);
}

int Logger::write(const QString &str)
{
    return this->write(str.toUtf8());
}

int Logger::writeWithDate(const QString &str)
{
    return this->writeWithDate(str.toUtf8());
}

int Logger::write(const QByteArray &bytes)
{
    int ret = 0;
    pthread_mutex_lock(&m_writeMutex);

    QTextStream out(&m_logFile);
    out << bytes << endl;
    ret = bytes.length();

    pthread_mutex_unlock(&m_writeMutex);

    return ret;
}

int Logger::writeWithDate(const QByteArray &bytes)
{
    QString str_dateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh:mm:ss ");
    QString str_write;
    str_write.append(str_dateTime);
    str_write.append(bytes);

    return this->write(str_write.toUtf8());
}

QByteArray Logger::read(uint64_t maxSize)
{
    QByteArray readBytes;
    pthread_mutex_lock(&m_readMutex);

    readBytes = m_logFile.read(maxSize);

    pthread_mutex_unlock(&m_readMutex);

    return readBytes;
}

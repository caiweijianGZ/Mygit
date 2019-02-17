#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QMap>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTextStream>
#include <QDateTime>
#include <pthread.h>

class Logger
{
public:
    static Logger* LoggerCreate(const QString &fileName);
    virtual ~Logger();
    int write(const char *str);
    int writeWithDate(const char *str);
    int write(const QString &str);
    int writeWithDate(const QString &str);
    int write(const QByteArray &bytes);
    int writeWithDate(const QByteArray &bytes);
    QByteArray read(uint64_t maxSize);

protected:
    explicit Logger(const QString &fileName);
    static bool fileCreate(const QString &fileName);

protected:
    pthread_mutex_t m_writeMutex;
    pthread_mutex_t m_readMutex;
    static QMap<QString, Logger*> m_loggerMap;
    QString m_fileName;
    QFile m_logFile;
};

#endif // LOGGER_H

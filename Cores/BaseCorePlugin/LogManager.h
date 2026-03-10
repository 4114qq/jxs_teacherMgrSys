#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include "BaseCorePlugin_global.h"
#include "../../common/interfaces/ILogManager.h"
#include <QObject>
#include <QMutex>
#include <QFile>
#include <QTextStream>
#include <QQueue>
#include <QStringList>
#include <QDate>

class BASECOREPLUGIN_EXPORT LogManager : public QObject, public ILogManager
{
    Q_OBJECT
    Q_INTERFACES(ILogManager)

public:
    explicit LogManager(QObject *parent = nullptr);
    ~LogManager();

    void logDebug(const QString &tag, const QString &message) override;
    void logInfo(const QString &tag, const QString &message) override;
    void logWarning(const QString &tag, const QString &message) override;
    void logError(const QString &tag, const QString &message) override;

    void setLogLevel(LogLevel level) override;
    LogLevel getLogLevel() const override;

    void setLogFilePath(const QString &path) override;
    QString getLogFilePath() const override;

    void clearLogs() override;
    QList<LogEntry> getLogs(int maxCount = 100) const override;

    QStringList getTags() const override;

signals:
    void newLogEntry(const LogEntry &entry);

private:
    void writeLog(LogLevel level, const QString &tag, const QString &message);
    void flushToFile();
    QString levelToString(LogLevel level) const;
    QString getLogFileName() const;

    LogLevel m_logLevel;
    QString m_logFilePath;
    mutable QMutex m_mutex;
    QFile *m_logFile;
    QTextStream *m_logStream;
    QQueue<LogEntry> m_logEntries;
    int m_maxEntries;
    QStringList m_tags;
};

#endif // LOGMANAGER_H

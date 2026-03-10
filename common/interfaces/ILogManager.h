#ifndef ILOGMANAGER_H
#define ILOGMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

struct LogEntry {
    QString timestamp;
    LogLevel level;
    QString tag;
    QString message;
};

class ILogManager
{
public:
    virtual ~ILogManager() = default;

    virtual void logDebug(const QString &tag, const QString &message) = 0;
    virtual void logInfo(const QString &tag, const QString &message) = 0;
    virtual void logWarning(const QString &tag, const QString &message) = 0;
    virtual void logError(const QString &tag, const QString &message) = 0;

    virtual void setLogLevel(LogLevel level) = 0;
    virtual LogLevel getLogLevel() const = 0;

    virtual void setLogFilePath(const QString &path) = 0;
    virtual QString getLogFilePath() const = 0;

    virtual void clearLogs() = 0;
    virtual QList<LogEntry> getLogs(int maxCount = 100) const = 0;

    virtual QStringList getTags() const = 0;
};

#define ILOGMANAGER_IID "com.jxs.teacherMgrSys.ILogManager"
Q_DECLARE_INTERFACE(ILogManager, ILOGMANAGER_IID)

#endif // ILOGMANAGER_H

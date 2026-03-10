#include "LogManager.h"
#include <QDir>
#include <QDateTime>
#include <QDebug>

LogManager::LogManager(QObject *parent)
    : QObject(parent)
    , m_logLevel(LogLevel::INFO)
    , m_logFilePath("./logs")
    , m_logFile(nullptr)
    , m_logStream(nullptr)
    , m_maxEntries(1000)
{
    QDir dir;
    if (!dir.exists(m_logFilePath)) {
        dir.mkpath(m_logFilePath);
    }

    QString fileName = getLogFileName();
    m_logFile = new QFile(m_logFilePath + "/" + fileName);
    if (m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        m_logStream = new QTextStream(m_logFile);
    }
}

LogManager::~LogManager()
{
    if (m_logStream) {
        m_logStream->flush();
        delete m_logStream;
        m_logStream = nullptr;
    }
    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }
}

void LogManager::logDebug(const QString &tag, const QString &message)
{
    writeLog(LogLevel::DEBUG, tag, message);
}

void LogManager::logInfo(const QString &tag, const QString &message)
{
    writeLog(LogLevel::INFO, tag, message);
}

void LogManager::logWarning(const QString &tag, const QString &message)
{
    writeLog(LogLevel::WARNING, tag, message);
}

void LogManager::logError(const QString &tag, const QString &message)
{
    writeLog(LogLevel::ERROR, tag, message);
}

void LogManager::writeLog(LogLevel level, const QString &tag, const QString &message)
{
    QMutexLocker locker(&m_mutex);

    if (level < m_logLevel) {
        return;
    }

    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    entry.level = level;
    entry.tag = tag;
    entry.message = message;

    if (m_logStream) {
        *m_logStream << QString("[%1] [%2] [%3] %4")
                        .arg(entry.timestamp)
                        .arg(levelToString(entry.level))
                        .arg(entry.tag)
                        .arg(entry.message) << "\n";
        m_logStream->flush();
    }

    m_logEntries.enqueue(entry);
    if (m_logEntries.size() > m_maxEntries) {
        m_logEntries.dequeue();
    }

    if (!m_tags.contains(tag)) {
        m_tags.append(tag);
    }

    emit newLogEntry(entry);
}

void LogManager::setLogLevel(LogLevel level)
{
    QMutexLocker locker(&m_mutex);
    m_logLevel = level;
}

LogLevel LogManager::getLogLevel() const
{
    return m_logLevel;
}

void LogManager::setLogFilePath(const QString &path)
{
    QMutexLocker locker(&m_mutex);

    if (m_logStream) {
        m_logStream->flush();
        delete m_logStream;
        m_logStream = nullptr;
    }
    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }

    m_logFilePath = path;

    QDir dir;
    if (!dir.exists(m_logFilePath)) {
        dir.mkpath(m_logFilePath);
    }

    QString fileName = getLogFileName();
    m_logFile = new QFile(m_logFilePath + "/" + fileName);
    if (m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        m_logStream = new QTextStream(m_logFile);
    }
}

QString LogManager::getLogFilePath() const
{
    return m_logFilePath;
}

void LogManager::clearLogs()
{
    QMutexLocker locker(&m_mutex);
    m_logEntries.clear();
}

QList<LogEntry> LogManager::getLogs(int maxCount) const
{
    QMutexLocker locker(&m_mutex);

    QList<LogEntry> result;
    int count = qMin(maxCount, m_logEntries.size());
    int start = m_logEntries.size() - count;

    for (int i = start; i < m_logEntries.size(); ++i) {
        result.append(m_logEntries.at(i));
    }

    return result;
}

QStringList LogManager::getTags() const
{
    return m_tags;
}

QString LogManager::levelToString(LogLevel level) const
{
    switch (level) {
    case LogLevel::DEBUG:   return "DEBUG";
    case LogLevel::INFO:    return "INFO";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR:   return "ERROR";
    default:                return "UNKNOWN";
    }
}

QString LogManager::getLogFileName() const
{
    return QDate::currentDate().toString("yyyy-MM-dd") + ".log";
}

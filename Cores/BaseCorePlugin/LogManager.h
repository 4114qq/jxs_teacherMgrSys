/**
 * @file LogManager.h
 * @brief 日志管理器实现
 * @details 提供日志记录、日志级别管理、日志文件管理等功能
 */

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

/**
 * @class LogManager
 * @brief 日志管理器
 * @details 提供日志记录、日志级别管理、日志文件管理等功能
 */
class BASECOREPLUGIN_EXPORT LogManager : public QObject, public ILogManager
{
    Q_OBJECT
    Q_INTERFACES(ILogManager)

public:
    // ===== 构造函数 =====

    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit LogManager(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~LogManager();

    // ===== 日志记录 =====

    /**
     * @brief 记录调试日志
     * @param tag 标签
     * @param message 消息
     */
    void logDebug(const QString &tag, const QString &message) override;

    /**
     * @brief 记录信息日志
     * @param tag 标签
     * @param message 消息
     */
    void logInfo(const QString &tag, const QString &message) override;

    /**
     * @brief 记录警告日志
     * @param tag 标签
     * @param message 消息
     */
    void logWarning(const QString &tag, const QString &message) override;

    /**
     * @brief 记录错误日志
     * @param tag 标签
     * @param message 消息
     */
    void logError(const QString &tag, const QString &message) override;

    // ===== 日志级别 =====

    /**
     * @brief 设置日志级别
     * @param level 日志级别
     */
    void setLogLevel(LogLevel level) override;

    /**
     * @brief 获取日志级别
     * @return LogLevel 日志级别
     */
    LogLevel getLogLevel() const override;

    // ===== 日志文件 =====

    /**
     * @brief 设置日志文件路径
     * @param path 路径
     */
    void setLogFilePath(const QString &path) override;

    /**
     * @brief 获取日志文件路径
     * @return QString 路径
     */
    QString getLogFilePath() const override;

    // ===== 日志查询 =====

    /**
     * @brief 清空日志
     */
    void clearLogs() override;

    /**
     * @brief 获取日志列表
     * @param maxCount 最大数量
     * @return QList<LogEntry> 日志列表
     */
    QList<LogEntry> getLogs(int maxCount = 100) const override;

    /**
     * @brief 获取标签列表
     * @return QStringList 标签列表
     */
    QStringList getTags() const override;

signals:
    /**
     * @brief 新日志条目信号
     * @param entry 日志条目
     */
    void newLogEntry(const LogEntry &entry);

private:
    /**
     * @brief 写入日志
     * @param level 日志级别
     * @param tag 标签
     * @param message 消息
     */
    void writeLog(LogLevel level, const QString &tag, const QString &message);

    /**
     * @brief 刷新到文件
     */
    void flushToFile();

    /**
     * @brief 级别转字符串
     * @param level 日志级别
     * @return QString 级别字符串
     */
    QString levelToString(LogLevel level) const;

    /**
     * @brief 获取日志文件名
     * @return QString 文件名
     */
    QString getLogFileName() const;

private:
    LogLevel m_logLevel;               ///< 日志级别
    QString m_logFilePath;             ///< 日志文件路径
    mutable QMutex m_mutex;            ///< 互斥锁
    QFile *m_logFile;                  ///< 日志文件
    QTextStream *m_logStream;          ///< 日志流
    QQueue<LogEntry> m_logEntries;    ///< 日志队列
    int m_maxEntries;                  ///< 最大条目数
    QStringList m_tags;                ///< 标签列表
};

#endif // LOGMANAGER_H

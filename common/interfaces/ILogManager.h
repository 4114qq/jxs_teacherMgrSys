/**
 * @file ILogManager.h
 * @brief 日志管理接口定义
 * @details 定义日志记录、级别管理、日志查询等功能的接口
 */

#ifndef ILOGMANAGER_H
#define ILOGMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @enum LogLevel
 * @brief 日志级别枚举
 */
enum class LogLevel {
    DEBUG,   ///< 调试级别
    INFO,    ///< 信息级别
    WARNING, ///< 警告级别
    ERROR    ///< 错误级别
};

/**
 * @struct LogEntry
 * @brief 日志条目结构体
 */
struct LogEntry {
    QString timestamp;  ///< 时间戳
    LogLevel level;    ///< 日志级别
    QString tag;        ///< 标签
    QString message;    ///< 日志消息
};

/**
 * @class ILogManager
 * @brief 日志管理器接口
 * @details 提供日志记录、级别管理、日志查询等功能
 */
class ILogManager
{
public:
    /**
     * @brief 析构函数
     */
    virtual ~ILogManager() = default;

    // ===== 日志记录 =====

    /**
     * @brief 记录调试日志
     * @param tag 标签
     * @param message 日志消息
     */
    virtual void logDebug(const QString &tag, const QString &message) = 0;

    /**
     * @brief 记录信息日志
     * @param tag 标签
     * @param message 日志消息
     */
    virtual void logInfo(const QString &tag, const QString &message) = 0;

    /**
     * @brief 记录警告日志
     * @param tag 标签
     * @param message 日志消息
     */
    virtual void logWarning(const QString &tag, const QString &message) = 0;

    /**
     * @brief 记录错误日志
     * @param tag 标签
     * @param message 日志消息
     */
    virtual void logError(const QString &tag, const QString &message) = 0;

    // ===== 级别管理 =====

    /**
     * @brief 设置日志级别
     * @param level 日志级别
     */
    virtual void setLogLevel(LogLevel level) = 0;

    /**
     * @brief 获取日志级别
     * @return LogLevel 日志级别
     */
    virtual LogLevel getLogLevel() const = 0;

    // ===== 文件配置 =====

    /**
     * @brief 设置日志文件路径
     * @param path 日志文件路径
     */
    virtual void setLogFilePath(const QString &path) = 0;

    /**
     * @brief 获取日志文件路径
     * @return QString 日志文件路径
     */
    virtual QString getLogFilePath() const = 0;

    // ===== 日志查询 =====

    /**
     * @brief 清除所有日志
     */
    virtual void clearLogs() = 0;

    /**
     * @brief 获取日志列表
     * @param maxCount 最大日志数量
     * @return QList<LogEntry> 日志列表
     */
    virtual QList<LogEntry> getLogs(int maxCount = 100) const = 0;

    /**
     * @brief 获取所有标签
     * @return QStringList 标签列表
     */
    virtual QStringList getTags() const = 0;
};

#define ILOGMANAGER_IID "com.jxs.teacherMgrSys.ILogManager"
Q_DECLARE_INTERFACE(ILogManager, ILOGMANAGER_IID)

#endif // ILOGMANAGER_H

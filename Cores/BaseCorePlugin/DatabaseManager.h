/**
 * @file DatabaseManager.h
 * @brief 数据库管理器实现
 * @details 提供数据库连接、查询、事务管理等操作
 */

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "BaseCorePlugin_global.h"
#include "../../common/interfaces/IDatabaseManager.h"
#include "../../common/interfaces/ILogManager.h"
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTimer>
#include <QMutex>
#include <QSharedPointer>

/**
 * @class DatabaseManager
 * @brief 数据库管理器
 * @details 实现数据库连接、查询执行、事务管理等功能
 */
class BASECOREPLUGIN_EXPORT DatabaseManager : public QObject, public IDatabaseManager
{
    Q_OBJECT
    Q_INTERFACES(IDatabaseManager)

public:
    // ===== 构造函数 =====

    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit DatabaseManager(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~DatabaseManager();

    /**
     * @brief 设置日志管理器
     * @param manager 日志管理器指针
     */
    void setLogManager(ILogManager *manager);

    // ===== 连接管理 =====

    /**
     * @brief 连接数据库
     * @param host 主机地址
     * @param port 端口号
     * @param dbName 数据库名称
     * @param user 用户名
     * @param password 密码
     * @return bool 连接是否成功
     */
    bool connect(const QString &host, int port, const QString &dbName,
                const QString &user, const QString &password) override;

    /**
     * @brief 断开数据库连接
     */
    void disconnect() override;

    /**
     * @brief 检查是否已连接
     * @return bool 是否已连接
     */
    bool isConnected() override;

    /**
     * @brief 重新连接
     * @return bool 重新连接是否成功
     */
    bool reconnect() override;

    /**
     * @brief 检查连接状态
     * @return bool 连接是否正常
     */
    bool checkConnection() override;

    // ===== 查询执行 =====

    /**
     * @brief 执行SQL语句
     * @param sql SQL语句
     * @return bool 执行是否成功
     */
    bool execute(const QString &sql) override;

    /**
     * @brief 执行带参数的SQL语句
     * @param sql SQL语句
     * @param params 参数映射
     * @return bool 执行是否成功
     */
    bool execute(const QString &sql, const QVariantMap &params) override;

    /**
     * @brief 执行查询
     * @param sql SQL语句
     * @param params 参数映射
     * @return QSharedPointer<QSqlQuery> 查询结果
     */
    QSharedPointer<QSqlQuery> query(const QString &sql, const QVariantMap &params = QVariantMap()) override;

    // ===== 事务管理 =====

    /**
     * @brief 开启事务
     * @return bool 是否成功
     */
    bool transaction() override;

    /**
     * @brief 提交事务
     * @return bool 是否成功
     */
    bool commit() override;

    /**
     * @brief 回滚事务
     * @return bool 是否成功
     */
    bool rollback() override;

    // ===== 信息获取 =====

    /**
     * @brief 获取最后一次错误信息
     * @return QString 错误信息
     */
    QString lastError() const override;

    /**
     * @brief 获取数据库类型
     * @return QString 数据库类型
     */
    QString databaseType() const override;

    /**
     * @brief 设置数据库类型
     * @param type 数据库类型
     */
    void setDatabaseType(const QString &type);

signals:
    /**
     * @brief 连接状态变化信号
     * @param connected 是否已连接
     */
    void connectionStatusChanged(bool connected);

    /**
     * @brief 错误发生信号
     * @param error 错误信息
     */
    void errorOccurred(const QString &error);

private:
    /**
     * @brief 创建数据库连接
     * @return bool 是否成功
     */
    bool createConnection();

    /**
     * @brief 启动健康检查
     */
    void startHealthCheck();

    /**
     * @brief 停止健康检查
     */
    void stopHealthCheck();

    /**
     * @brief 计划重连
     */
    void scheduleReconnect();

private:
    QString m_databaseType;           ///< 数据库类型
    QString m_host;                  ///< 主机地址
    int m_port;                     ///< 端口号
    QString m_dbName;               ///< 数据库名称
    QString m_user;                 ///< 用户名
    QString m_password;             ///< 密码

    QSqlDatabase m_db;               ///< 数据库连接
    QTimer *m_reconnectTimer;       ///< 重连定时器
    QTimer *m_healthCheckTimer;    ///< 健康检查定时器
    bool m_isConnected;            ///< 是否已连接
    QString m_lastError;            ///< 最后一次错误信息

    mutable QMutex m_mutex;         ///< 互斥锁
    QString m_connectionName;        ///< 连接名称
    ILogManager *m_logManager;      ///< 日志管理器
};

#endif // DATABASEMANAGER_H

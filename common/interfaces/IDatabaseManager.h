/**
 * @file IDatabaseManager.h
 * @brief 数据库管理接口定义
 * @details 定义数据库连接、查询、事务等操作的接口
 */

#ifndef IDATABASEMANAGER_H
#define IDATABASEMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QSharedPointer>

class QSqlQuery;

/**
 * @class IDatabaseManager
 * @brief 数据库管理器接口
 * @details 提供数据库连接、查询、事务管理等操作
 */
class IDatabaseManager
{
public:
    /**
     * @brief 析构函数
     */
    virtual ~IDatabaseManager() = default;

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
    virtual bool connect(const QString &host, int port, const QString &dbName,
                        const QString &user, const QString &password) = 0;

    /**
     * @brief 断开数据库连接
     */
    virtual void disconnect() = 0;

    /**
     * @brief 检查是否已连接
     * @return bool 是否已连接
     */
    virtual bool isConnected() = 0;

    /**
     * @brief 重新连接
     * @return bool 重新连接是否成功
     */
    virtual bool reconnect() = 0;

    /**
     * @brief 检查连接状态
     * @return bool 连接是否正常
     */
    virtual bool checkConnection() = 0;

    // ===== 查询执行 =====

    /**
     * @brief 执行SQL语句
     * @param sql SQL语句
     * @return bool 执行是否成功
     */
    virtual bool execute(const QString &sql) = 0;

    /**
     * @brief 执行带参数的SQL语句
     * @param sql SQL语句
     * @param params 参数映射
     * @return bool 执行是否成功
     */
    virtual bool execute(const QString &sql, const QVariantMap &params) = 0;

    /**
     * @brief 执行查询
     * @param sql SQL语句
     * @param params 参数映射
     * @return QSharedPointer<QSqlQuery> 查询结果
     */
    virtual QSharedPointer<QSqlQuery> query(const QString &sql, const QVariantMap &params = QVariantMap()) = 0;

    // ===== 事务管理 =====

    /**
     * @brief 开启事务
     * @return bool 开启是否成功
     */
    virtual bool transaction() = 0;

    /**
     * @brief 提交事务
     * @return bool 提交是否成功
     */
    virtual bool commit() = 0;

    /**
     * @brief 回滚事务
     * @return bool 回滚是否成功
     */
    virtual bool rollback() = 0;

    // ===== 信息获取 =====

    /**
     * @brief 获取最近错误信息
     * @return QString 错误信息
     */
    virtual QString lastError() const = 0;

    /**
     * @brief 获取数据库类型
     * @return QString 数据库类型（如 MySQL、SQLite）
     */
    virtual QString databaseType() const = 0;
};

#define IDATABASEMANAGER_IID "com.jxs.teacherMgrSys.IDatabaseManager"
Q_DECLARE_INTERFACE(IDatabaseManager, IDATABASEMANAGER_IID)

#endif // IDATABASEMANAGER_H

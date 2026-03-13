/**
 * @file AuthManager.h
 * @brief 认证管理实现
 * @details 提供用户登录、登出、权限验证、角色管理等功能
 */

#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include "BaseCorePlugin_global.h"
#include "../../common/interfaces/IAuthManager.h"
#include <QObject>
#include <QVariantMap>
#include <QString>
#include <QStringList>

class DatabaseManager;

/**
 * @class AuthManager
 * @brief 认证管理器
 * @details 实现用户认证、权限管理、角色分配等功能
 */
class BASECOREPLUGIN_EXPORT AuthManager : public QObject, public IAuthManager
{
    Q_OBJECT

public:
    // ===== 构造函数 =====

    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit AuthManager(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~AuthManager();

    // ===== 依赖设置 =====

    /**
     * @brief 设置数据库管理器
     * @param dbManager 数据库管理器指针
     */
    void setDatabaseManager(void *dbManager) override;

    // ===== 用户认证 =====

    /**
     * @brief 用户名密码登录
     * @param username 用户名
     * @param password 密码
     * @return bool 登录是否成功
     */
    bool login(const QString &username, const QString &password) override;

    /**
     * @brief 指纹登录
     * @param userId 用户ID
     * @param fingerprintData 指纹数据
     * @return bool 登录是否成功
     */
    bool loginByFingerprint(int userId, const QByteArray &fingerprintData) override;

    /**
     * @brief 人脸登录
     * @param userId 用户ID
     * @param faceData 人脸数据
     * @return bool 登录是否成功
     */
    bool loginByFace(int userId, const QByteArray &faceData) override;

    /**
     * @brief 登出
     */
    void logout() override;

    // ===== 登录状态 =====

    /**
     * @brief 检查是否已登录
     * @return bool 是否已登录
     */
    bool isLoggedIn() const override;

    /**
     * @brief 获取当前用户信息
     * @return QVariantMap 用户信息
     */
    QVariantMap getCurrentUser() const override;

    /**
     * @brief 获取当前登录类型
     * @return int 登录类型
     */
    int getCurrentLoginType() const override;

    // ===== 权限验证 =====

    /**
     * @brief 检查是否具有指定权限
     * @param permissionCode 权限代码
     * @return bool 是否具有权限
     */
    bool hasPermission(const QString &permissionCode) const override;

    /**
     * @brief 检查是否具有指定角色
     * @param roleName 角色名称
     * @return bool 是否具有角色
     */
    bool hasRole(const QString &roleName) const override;

    /**
     * @brief 获取用户权限列表
     * @param userId 用户ID
     * @return QStringList 权限列表
     */
    QStringList getUserPermissions(int userId) const override;

    /**
     * @brief 获取用户角色列表
     * @param userId 用户ID
     * @return QStringList 角色列表
     */
    QStringList getUserRoles(int userId) const override;

    // ===== 用户管理 =====

    /**
     * @brief 获取所有用户列表
     * @return QVariantList 用户列表
     */
    QVariantList getUsers() const override;

    /**
     * @brief 根据ID获取用户
     * @param id 用户ID
     * @return QVariantMap 用户信息
     */
    QVariantMap getUserById(int id) const override;

    /**
     * @brief 添加用户
     * @param user 用户信息
     * @return bool 添加是否成功
     */
    bool addUser(const QVariantMap &user) override;

    /**
     * @brief 更新用户
     * @param id 用户ID
     * @param user 用户信息
     * @return bool 更新是否成功
     */
    bool updateUser(int id, const QVariantMap &user) override;

    /**
     * @brief 删除用户
     * @param id 用户ID
     * @return bool 删除是否成功
     */
    bool deleteUser(int id) override;

    /**
     * @brief 重置密码
     * @param userId 用户ID
     * @param newPassword 新密码
     * @return bool 重置是否成功
     */
    bool resetPassword(int userId, const QString &newPassword) override;

    // ===== 生物特征管理 =====

    /**
     * @brief 设置用户指纹
     * @param userId 用户ID
     * @param fingerprintData 指纹数据
     * @return bool 设置是否成功
     */
    bool setFingerprint(int userId, const QByteArray &fingerprintData) override;

    /**
     * @brief 设置用户人脸数据
     * @param userId 用户ID
     * @param faceData 人脸数据
     * @return bool 设置是否成功
     */
    bool setFaceData(int userId, const QByteArray &faceData) override;

    /**
     * @brief 移除用户指纹
     * @param userId 用户ID
     * @return bool 移除是否成功
     */
    bool removeFingerprint(int userId) override;

    /**
     * @brief 移除用户人脸数据
     * @param userId 用户ID
     * @return bool 移除是否成功
     */
    bool removeFaceData(int userId) override;

    // ===== 角色管理 =====

    /**
     * @brief 获取所有角色列表
     * @return QVariantList 角色列表
     */
    QVariantList getRoles() const override;

    /**
     * @brief 添加角色
     * @param role 角色信息
     * @return bool 添加是否成功
     */
    bool addRole(const QVariantMap &role) override;

    /**
     * @brief 更新角色
     * @param id 角色ID
     * @param role 角色信息
     * @return bool 更新是否成功
     */
    bool updateRole(int id, const QVariantMap &role) override;

    /**
     * @brief 删除角色
     * @param id 角色ID
     * @return bool 删除是否成功
     */
    bool deleteRole(int id) override;

    /**
     * @brief 为用户分配角色
     * @param userId 用户ID
     * @param roleIds 角色ID列表
     * @return bool 分配是否成功
     */
    bool assignRolesToUser(int userId, const QList<int> &roleIds) override;

    // ===== 权限管理 =====

    /**
     * @brief 获取所有权限列表
     * @return QVariantList 权限列表
     */
    QVariantList getPermissions() const override;

    /**
     * @brief 添加权限
     * @param permission 权限信息
     * @return bool 添加是否成功
     */
    bool addPermission(const QVariantMap &permission) override;

    /**
     * @brief 删除权限
     * @param id 权限ID
     * @return bool 删除是否成功
     */
    bool deletePermission(int id) override;

    /**
     * @brief 为角色分配权限
     * @param roleId 角色ID
     * @param permissionIds 权限ID列表
     * @return bool 分配是否成功
     */
    bool assignPermissionsToRole(int roleId, const QList<int> &permissionIds) override;

private:
    /**
     * @brief 解析角色ID字符串
     * @param roleIdsStr 角色ID字符串（逗号分隔）
     */
    void parseRoleIds(const QString &roleIdsStr);

signals:
    /**
     * @brief 用户登录信号
     * @param user 用户信息
     */
    void userLoggedIn(const QVariantMap &user);

    /**
     * @brief 用户登出信号
     */
    void userLoggedOut();

    /**
     * @brief 登录失败信号
     * @param reason 失败原因
     */
    void loginFailed(const QString &reason);

private:
    DatabaseManager *m_databaseManager;    ///< 数据库管理器
    QVariantMap m_currentUser;              ///< 当前登录用户
    int m_loginType;                       ///< 当前登录类型
    QStringList m_currentPermissions;      ///< 当前用户权限列表
    QStringList m_currentRoles;            ///< 当前用户角色列表
};

#endif // AUTHMANAGER_H

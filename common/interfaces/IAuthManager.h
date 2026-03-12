/**
 * @file IAuthManager.h
 * @brief 认证管理接口定义
 * @details 定义用户认证、权限、角色管理的接口
 */

#ifndef IAUTHMANAGER_H
#define IAUTHMANAGER_H

#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QList>
#include <QByteArray>

/**
 * @class IAuthManager
 * @brief 认证管理器接口
 * @details 提供用户登录、登出、权限验证、角色管理等功能
 */
class IAuthManager
{
public:
    /**
     * @brief 登录类型枚举
     */
    enum LoginType {
        LoginType_Password,   ///< 密码登录
        LoginType_Fingerprint, ///< 指纹登录
        LoginType_Face        ///< 人脸登录
    };

    virtual ~IAuthManager() = default;

    // ===== 用户认证 =====

    /**
     * @brief 用户名密码登录
     * @param username 用户名
     * @param password 密码
     * @return bool 登录是否成功
     */
    virtual bool login(const QString &username, const QString &password) = 0;

    /**
     * @brief 指纹登录
     * @param userId 用户ID
     * @param fingerprintData 指纹数据
     * @return bool 登录是否成功
     */
    virtual bool loginByFingerprint(int userId, const QByteArray &fingerprintData) = 0;

    /**
     * @brief 人脸登录
     * @param userId 用户ID
     * @param faceData 人脸数据
     * @return bool 登录是否成功
     */
    virtual bool loginByFace(int userId, const QByteArray &faceData) = 0;

    /**
     * @brief 登出
     */
    virtual void logout() = 0;

    /**
     * @brief 检查是否已登录
     * @return bool 是否已登录
     */
    virtual bool isLoggedIn() const = 0;

    /**
     * @brief 获取当前登录用户信息
     * @return QVariantMap 用户信息
     */
    virtual QVariantMap getCurrentUser() const = 0;

    /**
     * @brief 获取当前登录类型
     * @return int 登录类型
     */
    virtual int getCurrentLoginType() const = 0;

    // ===== 权限验证 =====

    /**
     * @brief 检查用户是否拥有指定权限
     * @param permissionCode 权限代码
     * @return bool 是否拥有权限
     */
    virtual bool hasPermission(const QString &permissionCode) const = 0;

    /**
     * @brief 检查用户是否拥有指定角色
     * @param roleName 角色名称
     * @return bool 是否拥有角色
     */
    virtual bool hasRole(const QString &roleName) const = 0;

    /**
     * @brief 获取用户权限列表
     * @param userId 用户ID
     * @return QStringList 权限代码列表
     */
    virtual QStringList getUserPermissions(int userId) const = 0;

    /**
     * @brief 获取用户角色列表
     * @param userId 用户ID
     * @return QStringList 角色名称列表
     */
    virtual QStringList getUserRoles(int userId) const = 0;

    // ===== 用户管理 =====

    /**
     * @brief 获取所有用户列表
     * @return QVariantList 用户列表
     */
    virtual QVariantList getUsers() const = 0;

    /**
     * @brief 根据ID获取用户信息
     * @param id 用户ID
     * @return QVariantMap 用户信息
     */
    virtual QVariantMap getUserById(int id) const = 0;

    /**
     * @brief 添加用户
     * @param user 用户信息
     * @return bool 添加是否成功
     */
    virtual bool addUser(const QVariantMap &user) = 0;

    /**
     * @brief 更新用户信息
     * @param id 用户ID
     * @param user 用户信息
     * @return bool 更新是否成功
     */
    virtual bool updateUser(int id, const QVariantMap &user) = 0;

    /**
     * @brief 删除用户
     * @param id 用户ID
     * @return bool 删除是否成功
     */
    virtual bool deleteUser(int id) = 0;

    /**
     * @brief 重置用户密码
     * @param userId 用户ID
     * @param newPassword 新密码
     * @return bool 重置是否成功
     */
    virtual bool resetPassword(int userId, const QString &newPassword) = 0;

    // ===== 生物特征管理 =====

    /**
     * @brief 设置用户指纹
     * @param userId 用户ID
     * @param fingerprintData 指纹数据
     * @return bool 设置是否成功
     */
    virtual bool setFingerprint(int userId, const QByteArray &fingerprintData) = 0;

    /**
     * @brief 设置用户人脸数据
     * @param userId 用户ID
     * @param faceData 人脸数据
     * @return bool 设置是否成功
     */
    virtual bool setFaceData(int userId, const QByteArray &faceData) = 0;

    /**
     * @brief 删除用户指纹
     * @param userId 用户ID
     * @return bool 删除是否成功
     */
    virtual bool removeFingerprint(int userId) = 0;

    /**
     * @brief 删除用户人脸数据
     * @param userId 用户ID
     * @return bool 删除是否成功
     */
    virtual bool removeFaceData(int userId) = 0;

    // ===== 角色管理 =====

    /**
     * @brief 获取所有角色列表
     * @return QVariantList 角色列表
     */
    virtual QVariantList getRoles() const = 0;

    /**
     * @brief 添加角色
     * @param role 角色信息
     * @return bool 添加是否成功
     */
    virtual bool addRole(const QVariantMap &role) = 0;

    /**
     * @brief 更新角色
     * @param id 角色ID
     * @param role 角色信息
     * @return bool 更新是否成功
     */
    virtual bool updateRole(int id, const QVariantMap &role) = 0;

    /**
     * @brief 删除角色
     * @param id 角色ID
     * @return bool 删除是否成功
     */
    virtual bool deleteRole(int id) = 0;

    /**
     * @brief 为用户分配角色
     * @param userId 用户ID
     * @param roleIds 角色ID列表
     * @return bool 分配是否成功
     */
    virtual bool assignRolesToUser(int userId, const QList<int> &roleIds) = 0;

    // ===== 权限管理 =====

    /**
     * @brief 获取所有权限列表
     * @return QVariantList 权限列表
     */
    virtual QVariantList getPermissions() const = 0;

    /**
     * @brief 添加权限
     * @param permission 权限信息
     * @return bool 添加是否成功
     */
    virtual bool addPermission(const QVariantMap &permission) = 0;

    /**
     * @brief 删除权限
     * @param id 权限ID
     * @return bool 删除是否成功
     */
    virtual bool deletePermission(int id) = 0;

    /**
     * @brief 为角色分配权限
     * @param roleId 角色ID
     * @param permissionIds 权限ID列表
     * @return bool 分配是否成功
     */
    virtual bool assignPermissionsToRole(int roleId, const QList<int> &permissionIds) = 0;

    // ===== 依赖设置 =====

    /**
     * @brief 设置数据库管理器
     * @param dbManager 数据库管理器指针
     */
    virtual void setDatabaseManager(void *dbManager) = 0;
};

#endif // IAUTHMANAGER_H

#ifndef IAUTHMANAGER_H
#define IAUTHMANAGER_H

#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QList>
#include <QByteArray>

class IAuthManager
{
public:
    virtual ~IAuthManager() = default;

    enum LoginType {
        LoginType_Password,
        LoginType_Fingerprint,
        LoginType_Face
    };

    virtual bool login(const QString &username, const QString &password) = 0;
    virtual bool loginByFingerprint(int userId, const QByteArray &fingerprintData) = 0;
    virtual bool loginByFace(int userId, const QByteArray &faceData) = 0;
    virtual void logout() = 0;
    virtual bool isLoggedIn() const = 0;
    virtual QVariantMap getCurrentUser() const = 0;
    virtual int getCurrentLoginType() const = 0;

    virtual bool hasPermission(const QString &permissionCode) const = 0;
    virtual bool hasRole(const QString &roleName) const = 0;
    virtual QStringList getUserPermissions(int userId) const = 0;
    virtual QStringList getUserRoles(int userId) const = 0;

    virtual QVariantList getUsers() const = 0;
    virtual QVariantMap getUserById(int id) const = 0;
    virtual bool addUser(const QVariantMap &user) = 0;
    virtual bool updateUser(int id, const QVariantMap &user) = 0;
    virtual bool deleteUser(int id) = 0;
    virtual bool resetPassword(int userId, const QString &newPassword) = 0;
    virtual bool setFingerprint(int userId, const QByteArray &fingerprintData) = 0;
    virtual bool setFaceData(int userId, const QByteArray &faceData) = 0;
    virtual bool removeFingerprint(int userId) = 0;
    virtual bool removeFaceData(int userId) = 0;

    virtual QVariantList getRoles() const = 0;
    virtual bool addRole(const QVariantMap &role) = 0;
    virtual bool updateRole(int id, const QVariantMap &role) = 0;
    virtual bool deleteRole(int id) = 0;
    virtual bool assignRolesToUser(int userId, const QList<int> &roleIds) = 0;

    virtual QVariantList getPermissions() const = 0;
    virtual bool addPermission(const QVariantMap &permission) = 0;
    virtual bool deletePermission(int id) = 0;
    virtual bool assignPermissionsToRole(int roleId, const QList<int> &permissionIds) = 0;

    virtual void setDatabaseManager(void *dbManager) = 0;
};

#endif // IAUTHMANAGER_H

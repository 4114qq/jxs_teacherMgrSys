#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include "BaseCorePlugin_global.h"
#include "../../common/interfaces/IAuthManager.h"
#include <QObject>
#include <QVariantMap>
#include <QString>
#include <QStringList>

class DatabaseManager;

class BASECOREPLUGIN_EXPORT AuthManager : public QObject, public IAuthManager
{
    Q_OBJECT

public:
    explicit AuthManager(QObject *parent = nullptr);
    ~AuthManager();

    void setDatabaseManager(void *dbManager) override;

    bool login(const QString &username, const QString &password) override;
    bool loginByFingerprint(int userId, const QByteArray &fingerprintData) override;
    bool loginByFace(int userId, const QByteArray &faceData) override;
    void logout() override;
    bool isLoggedIn() const override;
    QVariantMap getCurrentUser() const override;
    int getCurrentLoginType() const override;

    bool hasPermission(const QString &permissionCode) const override;
    bool hasRole(const QString &roleName) const override;
    QStringList getUserPermissions(int userId) const override;
    QStringList getUserRoles(int userId) const override;

    QVariantList getUsers() const override;
    QVariantMap getUserById(int id) const override;
    bool addUser(const QVariantMap &user) override;
    bool updateUser(int id, const QVariantMap &user) override;
    bool deleteUser(int id) override;
    bool resetPassword(int userId, const QString &newPassword) override;
    bool setFingerprint(int userId, const QByteArray &fingerprintData) override;
    bool setFaceData(int userId, const QByteArray &faceData) override;
    bool removeFingerprint(int userId) override;
    bool removeFaceData(int userId) override;

    QVariantList getRoles() const override;
    bool addRole(const QVariantMap &role) override;
    bool updateRole(int id, const QVariantMap &role) override;
    bool deleteRole(int id) override;
    bool assignRolesToUser(int userId, const QList<int> &roleIds) override;

    QVariantList getPermissions() const override;
    bool addPermission(const QVariantMap &permission) override;
    bool deletePermission(int id) override;
    bool assignPermissionsToRole(int roleId, const QList<int> &permissionIds) override;

signals:
    void userLoggedIn(const QVariantMap &user);
    void userLoggedOut();
    void loginFailed(const QString &reason);

private:
    DatabaseManager *m_databaseManager;
    QVariantMap m_currentUser;
    int m_loginType;
    QStringList m_currentPermissions;
    QStringList m_currentRoles;
    void parseRoleIds(const QString &roleIdsStr);
};

#endif // AUTHMANAGER_H

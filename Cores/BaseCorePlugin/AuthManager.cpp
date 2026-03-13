/**
 * @file AuthManager.cpp
 * @brief 认证管理实现
 * @details 提供用户登录、登出、权限验证、角色管理等功能实现
 */

#include "AuthManager.h"
#include "DatabaseManager.h"
#include <QDebug>
#include <QDateTime>
#include <QCryptographicHash>

// ===== 构造函数 =====

AuthManager::AuthManager(QObject *parent)
    : QObject(parent)
    , m_databaseManager(nullptr)
    , m_loginType(LoginType_Password)
{
}

AuthManager::~AuthManager()
{
}

// ===== 依赖设置 =====

void AuthManager::setDatabaseManager(void *dbManager)
{
    m_databaseManager = static_cast<DatabaseManager*>(dbManager);
}

// ===== 用户认证 =====

bool AuthManager::login(const QString &username, const QString &password)
{
    if (!m_databaseManager) {
        qWarning() << "AuthManager: DatabaseManager not set";
        return false;
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(password.toUtf8());
    QString encryptedPassword = hash.result().toHex();

    QString sql = "SELECT * FROM MP_Person WHERE PersonNum = :username AND Password = :password ";
    QVariantMap params;
    params[":username"] = username;
    params[":password"] = encryptedPassword;

    auto result = m_databaseManager->query(sql, params);
    if (result && result->next()) {
        m_currentUser.clear();
        m_currentUser["id"] = result->value("ID").toInt();
        m_currentUser["PersonNum"] = result->value("PersonNum").toString();
        m_currentUser["Name"] = result->value("Name").toString();
        m_currentUser["RoleIDS"] = result->value("RoleIDS").toString();
        m_currentUser["TelephoneNum"] = result->value("TelephoneNum").toString();
        m_currentUser["Sex"] = result->value("Sex").toString();
        m_currentUser["Company"] = result->value("Company").toString();
        m_loginType = LoginType_Password;

        parseRoleIds(result->value("RoleIDS").toString());

        emit userLoggedIn(m_currentUser);
        return true;
    }

    emit loginFailed("Invalid username or password");
    return false;
}

bool AuthManager::loginByFingerprint(int userId, const QByteArray &fingerprintData)
{
    Q_UNUSED(fingerprintData);

    if (!m_databaseManager) {
        return false;
    }

    QString sql = "SELECT * FROM MP_Person WHERE ID = :id AND IsDel = 0";
    QVariantMap params;
    params[":id"] = userId;

    auto result = m_databaseManager->query(sql, params);
    if (result && result->next()) {
        m_currentUser.clear();
        m_currentUser["id"] = result->value("ID").toInt();
        m_currentUser["PersonNum"] = result->value("PersonNum").toString();
        m_currentUser["Name"] = result->value("Name").toString();
        m_currentUser["RoleIDS"] = result->value("RoleIDS").toString();
        m_loginType = LoginType_Fingerprint;

        parseRoleIds(result->value("RoleIDS").toString());

        emit userLoggedIn(m_currentUser);
        return true;
    }

    emit loginFailed("Invalid fingerprint");
    return false;
}

bool AuthManager::loginByFace(int userId, const QByteArray &faceData)
{
    Q_UNUSED(faceData);

    if (!m_databaseManager) {
        return false;
    }

    QString sql = "SELECT * FROM MP_Person WHERE ID = :id AND IsDel = 0";
    QVariantMap params;
    params[":id"] = userId;

    auto result = m_databaseManager->query(sql, params);
    if (result && result->next()) {
        m_currentUser.clear();
        m_currentUser["id"] = result->value("ID").toInt();
        m_currentUser["PersonNum"] = result->value("PersonNum").toString();
        m_currentUser["Name"] = result->value("Name").toString();
        m_currentUser["RoleIDS"] = result->value("RoleIDS").toString();
        m_loginType = LoginType_Face;

        parseRoleIds(result->value("RoleIDS").toString());

        emit userLoggedIn(m_currentUser);
        return true;
    }

    emit loginFailed("Invalid face data");
    return false;
}

void AuthManager::logout()
{
    m_currentUser.clear();
    m_currentRoles.clear();
    m_currentPermissions.clear();
    m_loginType = LoginType_Password;
    emit userLoggedOut();
}

// ===== 登录状态 =====

bool AuthManager::isLoggedIn() const
{
    return !m_currentUser.isEmpty() && m_currentUser.contains("id");
}

QVariantMap AuthManager::getCurrentUser() const
{
    return m_currentUser;
}

int AuthManager::getCurrentLoginType() const
{
    return m_loginType;
}

// ===== 权限验证 =====

void AuthManager::parseRoleIds(const QString &roleIdsStr)
{
    m_currentRoles.clear();
    m_currentPermissions.clear();

    QStringList ids = roleIdsStr.split(",", Qt::SkipEmptyParts);
    for (const QString &id : ids) {
        m_currentRoles.append(id.trimmed());
        m_currentPermissions.append(id.trimmed());
    }
}

bool AuthManager::hasPermission(const QString &permissionCode) const
{
    return m_currentPermissions.contains(permissionCode);
}

bool AuthManager::hasRole(const QString &roleName) const
{
    return m_currentRoles.contains(roleName);
}

QStringList AuthManager::getUserPermissions(int userId) const
{
    if (!m_databaseManager) {
        return QStringList();
    }

    QString sql = "SELECT RoleIDS FROM MP_Person WHERE ID = :id";
    auto result = m_databaseManager->query(sql, {{":id", userId}});
    if (result && result->next()) {
        QStringList perms;
        QStringList ids = result->value("RoleIDS").toString().split(",", Qt::SkipEmptyParts);
        for (const QString &id : ids) {
            perms.append(id.trimmed());
        }
        return perms;
    }
    return QStringList();
}

QStringList AuthManager::getUserRoles(int userId) const
{
    if (!m_databaseManager) {
        return QStringList();
    }

    QString sql = "SELECT RoleIDS FROM MP_Person WHERE ID = :id";
    auto result = m_databaseManager->query(sql, {{":id", userId}});
    if (result && result->next()) {
        QStringList roles;
        QStringList ids = result->value("RoleIDS").toString().split(",", Qt::SkipEmptyParts);
        for (const QString &id : ids) {
            roles.append(id.trimmed());
        }
        return roles;
    }
    return QStringList();
}

// ===== 用户管理 =====

QVariantList AuthManager::getUsers() const
{
    QVariantList users;
    if (!m_databaseManager) {
        return users;
    }

    QString sql = "SELECT ID, PersonNum, Name, Sex, TelephoneNum, Company, IsDel FROM MP_Person ORDER BY ID";
    auto result = m_databaseManager->query(sql);
    while (result && result->next()) {
        QVariantMap user;
        user["id"] = result->value("ID").toInt();
        user["PersonNum"] = result->value("PersonNum").toString();
        user["Name"] = result->value("Name").toString();
        user["Sex"] = result->value("Sex").toString();
        user["TelephoneNum"] = result->value("TelephoneNum").toString();
        user["Company"] = result->value("Company").toString();
        user["IsDel"] = result->value("IsDel").toInt();
        users.append(user);
    }
    return users;
}

QVariantMap AuthManager::getUserById(int id) const
{
    QVariantMap user;
    if (!m_databaseManager) {
        return user;
    }

    QString sql = "SELECT * FROM MP_Person WHERE ID = :id";
    auto result = m_databaseManager->query(sql, {{":id", id}});
    if (result && result->next()) {
        user["id"] = result->value("ID").toInt();
        user["PersonNum"] = result->value("PersonNum").toString();
        user["Password"] = result->value("Password").toString();
        user["Name"] = result->value("Name").toString();
        user["Sex"] = result->value("Sex").toString();
        user["TelephoneNum"] = result->value("TelephoneNum").toString();
        user["Company"] = result->value("Company").toString();
        user["RoleIDS"] = result->value("RoleIDS").toString();
    }
    return user;
}

bool AuthManager::addUser(const QVariantMap &user)
{
    if (!m_databaseManager) {
        return false;
    }

    QString sql = R"(
        INSERT INTO MP_Person (PersonNum, Password, Name, Sex, TelephoneNum, Company, RoleIDS, IsDel, CreateTime)
        VALUES (:PersonNum, :Password, :Name, :Sex, :TelephoneNum, :Company, :RoleIDS, 0, :CreateTime)
    )";

    QVariantMap params;
    params[":PersonNum"] = user.value("PersonNum");
    params[":Password"] = user.value("Password");
    params[":Name"] = user.value("Name");
    params[":Sex"] = user.value("Sex");
    params[":TelephoneNum"] = user.value("TelephoneNum");
    params[":Company"] = user.value("Company");
    params[":RoleIDS"] = user.value("RoleIDS");
    params[":CreateTime"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    return m_databaseManager->execute(sql, params);
}

bool AuthManager::updateUser(int id, const QVariantMap &user)
{
    if (!m_databaseManager) {
        return false;
    }

    QString sql = R"(
        UPDATE MP_Person SET
            PersonNum = :PersonNum,
            Password = :Password,
            Name = :Name,
            Sex = :Sex,
            TelephoneNum = :TelephoneNum,
            Company = :Company,
            RoleIDS = :RoleIDS,
            UpdateTime = :UpdateTime
        WHERE ID = :id
    )";

    QVariantMap params;
    params[":id"] = id;
    params[":PersonNum"] = user.value("PersonNum");
    params[":Password"] = user.value("Password");
    params[":Name"] = user.value("Name");
    params[":Sex"] = user.value("Sex");
    params[":TelephoneNum"] = user.value("TelephoneNum");
    params[":Company"] = user.value("Company");
    params[":RoleIDS"] = user.value("RoleIDS");
    params[":UpdateTime"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

    return m_databaseManager->execute(sql, params);
}

bool AuthManager::deleteUser(int id)
{
    if (!m_databaseManager) {
        return false;
    }

    return m_databaseManager->execute(
        "UPDATE MP_Person SET IsDel = 1 WHERE ID = :id",
        {{":id", id}}
    );
}

bool AuthManager::resetPassword(int userId, const QString &newPassword)
{
    if (!m_databaseManager) {
        return false;
    }

    return m_databaseManager->execute(
        "UPDATE MP_Person SET Password = :password, UpdateTime = :UpdateTime WHERE ID = :id",
        {{":password", newPassword}, {":id", userId}, {":UpdateTime", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")}}
    );
}

// ===== 生物特征管理 =====

bool AuthManager::setFingerprint(int userId, const QByteArray &fingerprintData)
{
    if (!m_databaseManager) {
        return false;
    }

    int fingerIndex = 1;
    QString sql = QString("UPDATE MP_Person SET Finger%1 = :data, UpdateTime = :UpdateTime WHERE ID = :id")
        .arg(fingerIndex);

    return m_databaseManager->execute(sql, {{":data", fingerprintData}, {":id", userId}, {":UpdateTime", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")}});
}

bool AuthManager::setFaceData(int userId, const QByteArray &faceData)
{
    if (!m_databaseManager) {
        return false;
    }

    return m_databaseManager->execute(
        "UPDATE MP_Person SET FaceData = :data, UpdateTime = :UpdateTime WHERE ID = :id",
        {{":data", faceData}, {":id", userId}, {":UpdateTime", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")}}
    );
}

bool AuthManager::removeFingerprint(int userId)
{
    if (!m_databaseManager) {
        return false;
    }

    return m_databaseManager->execute(
        "UPDATE MP_Person SET FingeCount = 0, Finger1 = NULL, Finger2 = NULL, Finger3 = NULL, Finger4 = NULL, Finger5 = NULL, Finger6 = NULL, Finger7 = NULL, Finger8 = NULL, Finger9 = NULL, Finger10 = NULL, UpdateTime = :UpdateTime WHERE ID = :id",
        {{":id", userId}, {":UpdateTime", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")}}
    );
}

bool AuthManager::removeFaceData(int userId)
{
    if (!m_databaseManager) {
        return false;
    }

    return m_databaseManager->execute(
        "UPDATE MP_Person SET FaceData = NULL, FaceLength = 0, UpdateTime = :UpdateTime WHERE ID = :id",
        {{":id", userId}, {":UpdateTime", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")}}
    );
}

// ===== 角色管理 =====

QVariantList AuthManager::getRoles() const
{
    return QVariantList();
}

bool AuthManager::addRole(const QVariantMap &role)
{
    Q_UNUSED(role);
    return false;
}

bool AuthManager::updateRole(int id, const QVariantMap &role)
{
    Q_UNUSED(id);
    Q_UNUSED(role);
    return false;
}

bool AuthManager::deleteRole(int id)
{
    Q_UNUSED(id);
    return false;
}

bool AuthManager::assignRolesToUser(int userId, const QList<int> &roleIds)
{
    if (!m_databaseManager) {
        return false;
    }

    QString roleIdsStr;
    for (int i = 0; i < roleIds.size(); ++i) {
        if (i > 0) roleIdsStr += ",";
        roleIdsStr += QString::number(roleIds[i]);
    }

    return m_databaseManager->execute(
        "UPDATE MP_Person SET RoleIDS = :roleIds, UpdateTime = :UpdateTime WHERE ID = :id",
        {{":roleIds", roleIdsStr}, {":id", userId}, {":UpdateTime", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")}}
    );
}

// ===== 权限管理 =====

QVariantList AuthManager::getPermissions() const
{
    return QVariantList();
}

bool AuthManager::addPermission(const QVariantMap &permission)
{
    Q_UNUSED(permission);
    return false;
}

bool AuthManager::deletePermission(int id)
{
    Q_UNUSED(id);
    return false;
}

bool AuthManager::assignPermissionsToRole(int roleId, const QList<int> &permissionIds)
{
    Q_UNUSED(roleId);
    Q_UNUSED(permissionIds);
    return false;
}

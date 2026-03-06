#ifndef IDATABASEMANAGER_H
#define IDATABASEMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QSharedPointer>

class QSqlQuery;

class IDatabaseManager
{
public:
    virtual ~IDatabaseManager() = default;

    virtual bool connect(const QString &host, int port, const QString &dbName,
                        const QString &user, const QString &password) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() = 0;

    virtual bool reconnect() = 0;
    virtual bool checkConnection() = 0;

    virtual bool execute(const QString &sql) = 0;
    virtual QSharedPointer<QSqlQuery> query(const QString &sql, const QVariantMap &params = QVariantMap()) = 0;

    virtual bool transaction() = 0;
    virtual bool commit() = 0;
    virtual bool rollback() = 0;

    virtual QString lastError() const = 0;
    virtual QString databaseType() const = 0;
};

#define IDATABASEMANAGER_IID "com.jxs.teacherMgrSys.IDatabaseManager"
Q_DECLARE_INTERFACE(IDatabaseManager, IDATABASEMANAGER_IID)

#endif // IDATABASEMANAGER_H

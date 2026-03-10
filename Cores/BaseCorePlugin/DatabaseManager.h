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

class BASECOREPLUGIN_EXPORT DatabaseManager : public QObject, public IDatabaseManager
{
    Q_OBJECT
    Q_INTERFACES(IDatabaseManager)

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    void setLogManager(ILogManager *manager);

    bool connect(const QString &host, int port, const QString &dbName,
                const QString &user, const QString &password) override;
    void disconnect() override;
    bool isConnected() override;

    bool reconnect() override;
    bool checkConnection() override;

    bool execute(const QString &sql) override;
    QSharedPointer<QSqlQuery> query(const QString &sql, const QVariantMap &params = QVariantMap()) override;

    bool transaction() override;
    bool commit() override;
    bool rollback() override;

    QString lastError() const override;
    QString databaseType() const override;

    void setDatabaseType(const QString &type);

signals:
    void connectionStatusChanged(bool connected);
    void errorOccurred(const QString &error);

private:
    bool createConnection();
    void startHealthCheck();
    void stopHealthCheck();
    void scheduleReconnect();

private:
    QString m_databaseType;
    QString m_host;
    int m_port;
    QString m_dbName;
    QString m_user;
    QString m_password;

    QSqlDatabase m_db;
    QTimer *m_reconnectTimer;
    QTimer *m_healthCheckTimer;
    bool m_isConnected;
    QString m_lastError;

    mutable QMutex m_mutex;
    QString m_connectionName;
    ILogManager *m_logManager;
};

#endif // DATABASEMANAGER_H

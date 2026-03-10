#include "DatabaseManager.h"
#include "../../common/interfaces/ILogManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QMutexLocker>
#include <QDebug>

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_databaseType("QODBC")
    , m_port(0)
    , m_reconnectTimer(new QTimer(this))
    , m_healthCheckTimer(new QTimer(this))
    , m_isConnected(false)
    , m_logManager(nullptr)
{
    m_connectionName = QString("DatabaseManager_%1").arg(reinterpret_cast<quint64>(this));

    QObject::connect(m_reconnectTimer, &QTimer::timeout, this, &DatabaseManager::reconnect);
    m_reconnectTimer->setSingleShot(true);

    QObject::connect(m_healthCheckTimer, &QTimer::timeout, this, &DatabaseManager::checkConnection);
}

DatabaseManager::~DatabaseManager()
{
    stopHealthCheck();
    disconnect();
}

void DatabaseManager::setLogManager(ILogManager *manager)
{
    m_logManager = manager;
}

bool DatabaseManager::connect(const QString &host, int port, const QString &dbName,
                             const QString &user, const QString &password)
{
    QMutexLocker locker(&m_mutex);

    m_host = host;
    m_port = port;
    m_dbName = dbName;
    m_user = user;
    m_password = password;

    return createConnection();
}

bool DatabaseManager::createConnection()
{
    if (m_db.isOpen()) {
        m_db.close();
    }

    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase tmpDb = QSqlDatabase::database(m_connectionName);
        if (tmpDb.isOpen()) {
            tmpDb.close();
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }

    m_db = QSqlDatabase::addDatabase(m_databaseType, m_connectionName);

    if (m_databaseType == "QODBC") {
        QString connectionString = QString("DRIVER={SQL Server};SERVER=%1,%2;DATABASE=%3;UID=%4;PWD=%5;")
                                      .arg(m_host)
                                      .arg(m_port)
                                      .arg(m_dbName)
                                      .arg(m_user)
                                      .arg(m_password);
        m_db.setDatabaseName(connectionString);
    } else {
        m_db.setHostName(m_host);
        m_db.setPort(m_port);
        m_db.setDatabaseName(m_dbName);
        m_db.setUserName(m_user);
        m_db.setPassword(m_password);
    }

    bool ok = m_db.open();
    if (ok) {
        m_isConnected = true;
        m_lastError.clear();
        startHealthCheck();
        emit connectionStatusChanged(true);
        if (m_logManager) {
            m_logManager->logInfo("DatabaseManager", "Database connected successfully");
        }
    } else {
        m_lastError = m_db.lastError().text();
        m_isConnected = false;
        emit errorOccurred(m_lastError);
        if (m_logManager) {
            m_logManager->logError("DatabaseManager", "Database connection failed: " + m_lastError);
        }
        scheduleReconnect();
    }

    return ok;
}

void DatabaseManager::disconnect()
{
    QMutexLocker locker(&m_mutex);

    stopHealthCheck();

    if (m_db.isOpen()) {
        m_db.close();
    }

    m_isConnected = false;
    emit connectionStatusChanged(false);
}

bool DatabaseManager::isConnected()
{
    return m_isConnected && m_db.isOpen();
}

bool DatabaseManager::reconnect()
{
    QMutexLocker locker(&m_mutex);

    if (m_logManager) {
        m_logManager->logWarning("DatabaseManager", "Attempting to reconnect to database...");
    }
    return createConnection();
}

bool DatabaseManager::checkConnection()
{
    QMutexLocker locker(&m_mutex);

    if (!m_db.isOpen()) {
        m_isConnected = false;
        emit connectionStatusChanged(false);
        scheduleReconnect();
        return false;
    }

    QSqlQuery query(m_db);
    bool ok = query.exec("SELECT 1");

    if (!ok || query.lastError().type() != QSqlError::NoError) {
        m_lastError = query.lastError().text();
        m_isConnected = false;
        emit connectionStatusChanged(false);
        emit errorOccurred(m_lastError);
        if (m_logManager) {
            m_logManager->logError("DatabaseManager", "Database connection check failed: " + m_lastError);
        }
        scheduleReconnect();
        return false;
    }

    if (!m_isConnected) {
        m_isConnected = true;
        emit connectionStatusChanged(true);
        if (m_logManager) {
            m_logManager->logInfo("DatabaseManager", "Database connection restored");
        }
    }

    return true;
}

bool DatabaseManager::execute(const QString &sql)
{
    QMutexLocker locker(&m_mutex);

    if (!m_db.isOpen()) {
        m_lastError = "Database is not connected";
        return false;
    }

    QSqlQuery query(m_db);
    bool ok = query.exec(sql);

    if (!ok) {
        m_lastError = query.lastError().text();
        emit errorOccurred(m_lastError);
        if (m_logManager) {
            m_logManager->logError("DatabaseManager", QString("SQL execute failed: %1, SQL: %2").arg(m_lastError, sql));
        }
    }

    return ok;
}

QSharedPointer<QSqlQuery> DatabaseManager::query(const QString &sql, const QVariantMap &params)
{
    QMutexLocker locker(&m_mutex);

    QSharedPointer<QSqlQuery> query(new QSqlQuery(m_db));

    if (!m_db.isOpen()) {
        m_lastError = "Database is not connected";
        return query;
    }

    query->prepare(sql);

    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        query->bindValue(it.key(), it.value());
    }

    bool ok = query->exec();

    if (!ok) {
        m_lastError = query->lastError().text();
        emit errorOccurred(m_lastError);
        if (m_logManager) {
            m_logManager->logError("DatabaseManager", QString("SQL query failed: %1, SQL: %2").arg(m_lastError, sql));
        }
    }

    return query;
}

bool DatabaseManager::transaction()
{
    QMutexLocker locker(&m_mutex);

    if (!m_db.isOpen()) {
        m_lastError = "Database is not connected";
        return false;
    }

    return m_db.transaction();
}

bool DatabaseManager::commit()
{
    QMutexLocker locker(&m_mutex);

    if (!m_db.isOpen()) {
        m_lastError = "Database is not connected";
        return false;
    }

    return m_db.commit();
}

bool DatabaseManager::rollback()
{
    QMutexLocker locker(&m_mutex);

    if (!m_db.isOpen()) {
        m_lastError = "Database is not connected";
        return false;
    }

    return m_db.rollback();
}

QString DatabaseManager::lastError() const
{
    QMutexLocker locker(&m_mutex);
    return m_lastError;
}

QString DatabaseManager::databaseType() const
{
    return m_databaseType;
}

void DatabaseManager::setDatabaseType(const QString &type)
{
    if (type.toLower() == "sqlserver" || type.toLower() == "mssql") {
        m_databaseType = "QODBC";
    } else if (type.toLower() == "mysql") {
        m_databaseType = "QMYSQL";
    } else {
        m_databaseType = type;
    }
}

void DatabaseManager::startHealthCheck()
{
    if (!m_healthCheckTimer->isActive()) {
        m_healthCheckTimer->start(30000);
    }
}

void DatabaseManager::stopHealthCheck()
{
    if (m_healthCheckTimer->isActive()) {
        m_healthCheckTimer->stop();
    }
}

void DatabaseManager::scheduleReconnect()
{
}

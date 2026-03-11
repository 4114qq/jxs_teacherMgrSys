#include "basecoreplugin.h"
#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/ILogManager.h"
#include "EventManager.h"
#include <QWidget>

BaseCorePlugin::BaseCorePlugin(QObject *parent)
    : QObject(parent),
    m_pluginWidget(nullptr),
    m_core(this),
    m_eventManager(new CoreEventBus()),
    m_databaseManager(new DatabaseManager(this)),
    m_logManager(new LogManager(this)),
    m_configManager(new ConfigManager(this)),
    m_authManager(new AuthManager(this))
{
}

BaseCorePlugin::~BaseCorePlugin()
{
    cleanup();
}

QString BaseCorePlugin::name() const
{
    return "BaseCorePlugin";
}

QString BaseCorePlugin::version() const
{
    return "1.0.0";
}

QString BaseCorePlugin::description() const
{
    return "";
}

QString BaseCorePlugin::author() const
{
    return "JXS Team";
}

bool BaseCorePlugin::initialize()
{
    if (m_configManager) {
        m_configManager->setLogManager(m_logManager);
        m_configManager->load();
    }
    loadConfig();
    return true;
}

bool BaseCorePlugin::startPlugin()
{
    if (m_configManager) {
        QString dbType = m_configManager->get("Database_Type", "MySQL").toString();
        QString dbHost = m_configManager->get("Database_Host", "localhost").toString();
        int dbPort = m_configManager->get("Database_Port", 3306).toInt();
        QString dbName = m_configManager->get("Database_Name", "testdb").toString();
        QString dbUser = m_configManager->get("Database_User", "root").toString();
        QString dbPassword = m_configManager->get("Database_Password", "").toString();

        if (m_databaseManager) {
            m_databaseManager->setLogManager(m_logManager);
            m_databaseManager->setDatabaseType(dbType);
            m_databaseManager->connect(dbHost, dbPort, dbName, dbUser, dbPassword);
        }

        if (m_authManager) {
            m_authManager->setDatabaseManager(m_databaseManager);
        }

        if (m_logManager) {
            QString logLevelStr = m_configManager->get("Log_Level", "info").toString();
            LogLevel logLevel = LogLevel::INFO;
            if (logLevelStr.toLower() == "debug") {
                logLevel = LogLevel::DEBUG;
            } else if (logLevelStr.toLower() == "warning") {
                logLevel = LogLevel::WARNING;
            } else if (logLevelStr.toLower() == "error") {
                logLevel = LogLevel::ERROR;
            }
            m_logManager->setLogLevel(logLevel);

            QString logPath = m_configManager->get("Log_Path", "./logs").toString();
            m_logManager->setLogFilePath(logPath);
        }
    }
    return true;
}

bool BaseCorePlugin::stopPlugin()
{
    return true;
}

void BaseCorePlugin::cleanup()
{
    if (m_databaseManager) {
        m_databaseManager->disconnect();
    }
    saveConfig();
}

QVariant BaseCorePlugin::getConfig(const QString &key, const QVariant &defaultValue) const
{
    if (m_config.contains(key)) {
        return m_config[key];
    }
    return defaultValue;
}

void BaseCorePlugin::setConfig(const QString &key, const QVariant &value)
{
    m_config[key] = value;
}

bool BaseCorePlugin::loadConfig()
{
    m_config["enabled"] = true;
    m_config["logLevel"] = "info";
    return true;
}

bool BaseCorePlugin::saveConfig()
{
    if (m_configManager) {
        m_configManager->save();
    }
    return true;
}

IBaseEventBus *BaseCorePlugin::eventManager() const
{
    return m_eventManager;
}

QStringList BaseCorePlugin::dependencies() const
{
    return QStringList();
}

IDatabaseManager *BaseCorePlugin::databaseManager() const
{
    return m_databaseManager;
}

ILogManager *BaseCorePlugin::logManager() const
{
    return m_logManager;
}

IAuthManager *BaseCorePlugin::authManager() const
{
    return m_authManager;
}

IConfigManager *BaseCorePlugin::configManager() const
{
    return m_configManager;
}

void BaseCorePlugin::setCore(IBasePlugin *core)
{
    m_core = core;
}

IBasePlugin *BaseCorePlugin::core() const
{
    return m_core;
}

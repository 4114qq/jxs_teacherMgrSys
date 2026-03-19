/**
 * @file basecoreplugin.cpp
 * @brief 基础核心插件实现文件
 */

#include "basecoreplugin.h"

BaseCorePlugin::BaseCorePlugin(QObject *parent)
    : QObject(parent),
    m_core(this),
    m_eventManager(new CoreEventBus()),
    m_databaseManager(new DatabaseManager(this)),
    m_logManager(new LogManager(this)),
    m_configManager(new ConfigManager(this)),
    m_authManager(new AuthManager(this)),
    m_httpClientManager(new HttpClientManager(this)),
    m_themeManager(new ThemeManager(this))
{
}

BaseCorePlugin::~BaseCorePlugin()
{
    cleanup();
}

// ===== 插件基本信息 =====

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

// ===== 核心插件设置 =====

void BaseCorePlugin::setCore(IBasePlugin *core)
{
    m_core = core;
}

IBasePlugin *BaseCorePlugin::core() const
{
    return m_core;
}

// ===== 依赖管理 =====

QStringList BaseCorePlugin::dependencies() const
{
    return QStringList();
}

// ===== 核心服务获取 =====

IBaseEventBus *BaseCorePlugin::eventManager() const
{
    return m_eventManager;
}

IDatabaseManager *BaseCorePlugin::databaseManager() const
{
    return m_databaseManager;
}

IConfigManager *BaseCorePlugin::configManager() const
{
    return m_configManager;
}

ILogManager *BaseCorePlugin::logManager() const
{
    return m_logManager;
}

IAuthManager *BaseCorePlugin::authManager() const
{
    return m_authManager;
}

IHttpClientManager *BaseCorePlugin::httpClientManager() const
{
    return m_httpClientManager;
}

IThemeManager *BaseCorePlugin::themeManager() const
{
    return m_themeManager;
}

// ===== 插件生命周期管理 =====

bool BaseCorePlugin::initialize()
{
    if (m_configManager) {
        m_configManager->setLogManager(m_logManager);
        m_configManager->load();
    }

    if (m_themeManager) {
        m_themeManager->initialize(m_configManager, m_eventManager);
    }

    return true;
}

bool BaseCorePlugin::startPlugin()
{
    if (m_configManager) {
        if (m_databaseManager) {
            QString dbType = m_configManager->get("Database.Type", "MySQL").toString();
            QString dbHost = m_configManager->get("Database.Host", "localhost").toString();
            int dbPort = m_configManager->get("Database.Port", 3306).toInt();
            QString dbName = m_configManager->get("Database.Name", "testdb").toString();
            QString dbUser = m_configManager->get("Database.User", "root").toString();
            QString dbPassword = m_configManager->get("Database.Password", "").toString();

            m_databaseManager->setLogManager(m_logManager);
            m_databaseManager->setDatabaseType(dbType);
            m_databaseManager->connect(dbHost, dbPort, dbName, dbUser, dbPassword);
        }

        if (m_authManager) {
            m_authManager->setDatabaseManager(m_databaseManager);
        }

        if (m_logManager) {
            QString logLevelStr = m_configManager->get("Log.Level", "info").toString();
            LogLevel logLevel = LogLevel::INFO;
            if (logLevelStr.toLower() == "debug") {
                logLevel = LogLevel::DEBUG;
            } else if (logLevelStr.toLower() == "warning") {
                logLevel = LogLevel::WARNING;
            } else if (logLevelStr.toLower() == "error") {
                logLevel = LogLevel::ERROR;
            }
            m_logManager->setLogLevel(logLevel);

            QString logPath = m_configManager->get("Log.Path", "./logs").toString();
            m_logManager->setLogFilePath(logPath);
        }

        if (m_httpClientManager) {
            QString httpIp = m_configManager->get("HTTP.IP", "192.168.1.10").toString();
            int httpPort = m_configManager->get("HTTP.Port", 8094).toInt();
            int httpTimeout = m_configManager->get("HTTP.Timeout", 30).toInt();
            QString httpToken = m_configManager->get("HTTP.Token", "").toString();

            m_httpClientManager->setServerUrl(httpIp, httpPort);
            m_httpClientManager->setTimeout(httpTimeout);
            if (!httpToken.isEmpty()) {
                m_httpClientManager->setToken(httpToken);
            }
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
}

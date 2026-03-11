#include "loguiplugin.h"
#include "logwidget.h"
#include <QIcon>

LogUIPlugin::LogUIPlugin(QObject *parent)
    : QObject(parent)
    , m_core(nullptr)
    , m_logWidget(nullptr)
    , m_logManager(nullptr)
    , m_authManager(nullptr)
    , m_databaseManager(nullptr)
    , m_eventManager(nullptr)
{
}

LogUIPlugin::~LogUIPlugin()
{
    if (m_logWidget) {
        delete m_logWidget;
        m_logWidget = nullptr;
    }
}

QString LogUIPlugin::name() const
{
    return "LogUIPlugin";
}

QString LogUIPlugin::version() const
{
    return "1.0.0";
}

QString LogUIPlugin::description() const
{
    return "Log Management UI Plugin";
}

bool LogUIPlugin::initialize()
{
    m_logWidget = new LogWidget();
    if (m_logManager) {
        m_logWidget->setLogManager(m_logManager);
    }
    return true;
}

bool LogUIPlugin::startPlugin()
{
    return true;
}

bool LogUIPlugin::stopPlugin()
{
    return true;
}

void LogUIPlugin::cleanup()
{
}

IBaseEventBus *LogUIPlugin::eventManager() const
{
    return m_eventManager;
}

IDatabaseManager *LogUIPlugin::databaseManager() const
{
    return m_databaseManager;
}

ILogManager *LogUIPlugin::logManager() const
{
    return m_logManager;
}

IAuthManager *LogUIPlugin::authManager() const
{
    return m_authManager;
}

IConfigManager *LogUIPlugin::configManager() const
{
    return nullptr;
}

QStringList LogUIPlugin::dependencies() const
{
    return QStringList() << "BaseCorePlugin";
}

QString LogUIPlugin::author() const
{
    return "Developer";
}

QVariant LogUIPlugin::getConfig(const QString &key, const QVariant &defaultValue) const
{
    return m_config.value(key, defaultValue);
}

void LogUIPlugin::setConfig(const QString &key, const QVariant &value)
{
    m_config[key] = value;

    if (key == "logManagerPtr") {
        m_logManager = static_cast<ILogManager*>(value.value<void*>());
        if (m_logWidget && m_logManager) {
            m_logWidget->setLogManager(m_logManager);
        }
    } else if (key == "databaseManagerPtr") {
        m_databaseManager = static_cast<IDatabaseManager*>(value.value<void*>());
    } else if (key == "eventManagerPtr") {
        m_eventManager = static_cast<IBaseEventBus*>(value.value<void*>());
    }
}

bool LogUIPlugin::loadConfig()
{
    return true;
}

bool LogUIPlugin::saveConfig()
{
    return true;
}

QWidget *LogUIPlugin::widget() const
{
    return m_logWidget;
}

QString LogUIPlugin::widgetTitle() const
{
    return QString::fromLocal8Bit("日志管理");
}

QIcon LogUIPlugin::widgetIcon() const
{
    return QIcon();
}

void LogUIPlugin::setLogManager(ILogManager *manager)
{
    m_logManager = manager;
    if (m_logWidget) {
        m_logWidget->setLogManager(manager);
    }
}

void LogUIPlugin::setCore(IBasePlugin *core)
{
    m_core = core;

    if (m_core) {
        m_logManager = m_core->logManager();
        if (m_logWidget && m_logManager) {
            m_logWidget->setLogManager(m_logManager);
        }
    }
}

IBasePlugin *LogUIPlugin::core() const
{
    return m_core;
}

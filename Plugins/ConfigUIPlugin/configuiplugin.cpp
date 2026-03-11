#include "configuiplugin.h"
#include "configwidget.h"
#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IConfigManager.h"
#include "../../common/interfaces/ILogManager.h"

ConfigUIPlugin::ConfigUIPlugin(QObject *parent)
    : QObject(parent)
    , m_core(nullptr)
    , m_configWidget(nullptr)
    , m_eventManager(nullptr)
    , m_databaseManager(nullptr)
    , m_logManager(nullptr)
    , m_authManager(nullptr)
    , m_configManager(nullptr)
{
}

ConfigUIPlugin::~ConfigUIPlugin()
{
    cleanup();
}

QString ConfigUIPlugin::name() const
{
    return "ConfigUIPlugin";
}

QString ConfigUIPlugin::version() const
{
    return "1.0.0";
}

QString ConfigUIPlugin::description() const
{
    return "配置管理界面插件";
}

QString ConfigUIPlugin::author() const
{
    return "Your Name";
}

QVariant ConfigUIPlugin::getConfig(const QString &key, const QVariant &defaultValue) const
{
    if (m_configManager) {
        return m_configManager->get(key, defaultValue);
    }
    return defaultValue;
}

void ConfigUIPlugin::setConfig(const QString &key, const QVariant &value)
{
    if (key == "configManagerPtr") {
        m_configManager = static_cast<IConfigManager*>(value.value<void*>());
        if (m_configWidget && m_configManager) {
            m_configWidget->setConfigManager(m_configManager);
        }
    } else if (m_configManager) {
        m_configManager->set(key, value);
    }
}

bool ConfigUIPlugin::loadConfig()
{
    if (m_configManager) {
        return m_configManager->load();
    }
    return false;
}

bool ConfigUIPlugin::saveConfig()
{
    if (m_configManager) {
        return m_configManager->save();
    }
    return false;
}

bool ConfigUIPlugin::initialize()
{
    m_configWidget = new ConfigWidget();
    m_configWidget->setWindowFlags(Qt::Widget);
    return true;
}

void ConfigUIPlugin::setConfigManager(IConfigManager *configManager)
{
    m_configManager = configManager;
    if (m_configWidget) {
        m_configWidget->setConfigManager(m_configManager);
    }
}

bool ConfigUIPlugin::startPlugin()
{
    return true;
}

bool ConfigUIPlugin::stopPlugin()
{
    return true;
}

void ConfigUIPlugin::cleanup()
{
    if (m_configWidget) {
        delete m_configWidget;
        m_configWidget = nullptr;
    }
}

void ConfigUIPlugin::setEventManager(IBaseEventBus *eventManager)
{
    m_eventManager = eventManager;
}

IBaseEventBus *ConfigUIPlugin::eventManager() const
{
    return m_eventManager;
}

IDatabaseManager *ConfigUIPlugin::databaseManager() const
{
    return m_databaseManager;
}

ILogManager *ConfigUIPlugin::logManager() const
{
    return m_logManager;
}

IAuthManager *ConfigUIPlugin::authManager() const
{
    return m_authManager;
}

IConfigManager *ConfigUIPlugin::configManager() const
{
    return m_configManager;
}

QWidget *ConfigUIPlugin::widget() const
{
    return m_configWidget;
}

QString ConfigUIPlugin::widgetTitle() const
{
    return "配置管理";
}

QIcon ConfigUIPlugin::widgetIcon() const
{
    return QIcon();
}

QStringList ConfigUIPlugin::dependencies() const
{
    return {"BaseCorePlugin"};
}

void ConfigUIPlugin::setCore(IBasePlugin *core)
{
    m_core = core;

    if (m_core) {
        m_configManager = m_core->configManager();
        if (m_configWidget && m_configManager) {
            m_configWidget->setConfigManager(m_configManager);
        }
    }
}

IBasePlugin *ConfigUIPlugin::core() const
{
    return m_core;
}

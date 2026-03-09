#include "configuiplugin.h"
#include "configwidget.h"
#include <QIcon>

ConfigUIPlugin::ConfigUIPlugin(QObject *parent)
    : QObject(parent)
    , m_configWidget(nullptr)
    , m_eventManager(nullptr)
    , m_databaseManager(nullptr)
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
    if (m_configManager) {
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

IBaseEventBus *ConfigUIPlugin::eventManager() const
{
    return m_eventManager;
}

IDatabaseManager *ConfigUIPlugin::databaseManager() const
{
    return m_databaseManager;
}

IConfigManager *ConfigUIPlugin::configManager() const
{
    return m_configManager;
}

QWidget *ConfigUIPlugin::mainWidget() const
{
    return m_configWidget;
}

QStringList ConfigUIPlugin::dependencies() const
{
    return {"BaseCorePlugin"};
}

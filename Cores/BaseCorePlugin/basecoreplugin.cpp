#include "basecoreplugin.h"

BaseCorePlugin::BaseCorePlugin(QObject *parent)
    : QObject(parent),
    m_eventManager(new CoreEventBus)
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
    loadConfig();
    return true;
}

bool BaseCorePlugin::startPlugin()
{
    return true;
}

bool BaseCorePlugin::stopPlugin()
{
    return true;
}

void BaseCorePlugin::cleanup()
{
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
    return true;
}

IBaseEventBus *BaseCorePlugin::eventManager() const
{
    return m_eventManager;
}

QStringList BaseCorePlugin::dependencies() const
{
    // 基础插件没有依赖
    return QStringList();
}

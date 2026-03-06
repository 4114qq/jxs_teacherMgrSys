#include "pluginmanager.h"
#include <QDir>
#include <QDebug>
#include <QVariantMap>

PluginManager::PluginManager(QObject *parent) : QObject(parent)
{
}

PluginManager::~PluginManager()
{
    unloadAllPlugins();
}

bool PluginManager::loadPlugins(const QString &pluginsPath)
{
    QDir pluginsDir(pluginsPath);
    if (!pluginsDir.exists()) {
        qWarning() << "Plugins directory not found:" << pluginsPath;
        return false;
    }

    bool success = true;
    for (const QString &fileName : pluginsDir.entryList(QDir::Files)) {
        if (QLibrary::isLibrary(fileName)) {
            QString pluginPath = pluginsDir.absoluteFilePath(fileName);
            if (!loadPlugin(pluginPath)) {
                success = false;
            }
        }
    }
    return success;
}

bool PluginManager::loadPlugin(const QString &pluginPath)
{
    QPluginLoader *loader = new QPluginLoader(pluginPath);
    QObject *pluginObject = loader->instance();

    if (!pluginObject) {
        qWarning() << "Failed to load plugin:" << pluginPath << "Error:" << loader->errorString();
        delete loader;
        emit pluginError(pluginPath, loader->errorString());
        return false;
    }

    IBasePlugin *plugin = qobject_cast<IBasePlugin*>(pluginObject);
    if (!plugin) {
        qWarning() << "Plugin does not implement IBasePlugin interface:" << pluginPath;
        loader->unload();
        delete loader;
        emit pluginError(pluginPath, "Plugin does not implement IBasePlugin interface");
        return false;
    }

    // 初始化插件
    if (!plugin->initialize()) {
        qWarning() << "Failed to initialize plugin:" << plugin->name();
        loader->unload();
        delete loader;
        emit pluginError(plugin->name(), "Failed to initialize plugin");
        return false;
    }

    if (!plugin->startPlugin()) {
        qWarning() << "Failed to start plugin:" << plugin->name();
        plugin->cleanup();
        loader->unload();
        delete loader;
        emit pluginError(plugin->name(), "Failed to start plugin");
        return false;
    }

    QString pluginName = plugin->name();
    m_mapPluginLoaders[pluginName] = loader;
    m_plugins[pluginName] = plugin;

    qInfo() << "Plugin loaded successfully:" << pluginName;
    emit pluginLoaded(pluginName);
    return true;
}

bool PluginManager::unloadPlugin(const QString &pluginName)
{
    if (!m_plugins.contains(pluginName)) {
        return false;
    }

    IBasePlugin *plugin = m_plugins[pluginName];
    QPluginLoader *loader = m_mapPluginLoaders[pluginName];

    // 停止插件
    plugin->stopPlugin();
    // 清理插件
    plugin->cleanup();
    // 卸载插件
    loader->unload();

    // 移除映射
    m_plugins.remove(pluginName);
    m_mapPluginLoaders.remove(pluginName);

    delete loader;

    qInfo() << "Plugin unloaded:" << pluginName;
    emit pluginUnloaded(pluginName);
    return true;
}

void PluginManager::unloadAllPlugins()
{
    QStringList pluginNames = m_plugins.keys();
    for (const QString &pluginName : pluginNames) {
        unloadPlugin(pluginName);
    }
}

IBasePlugin *PluginManager::getPlugin(const QString &pluginName) const
{
    return m_plugins.value(pluginName, nullptr);
}

QList<IBasePlugin *> PluginManager::getAllPlugins() const
{
    return m_plugins.values();
}

QStringList PluginManager::getPluginNames() const
{
    return m_plugins.keys();
}

bool PluginManager::triggerEvent(const QString &eventName, const QVariantMap &eventData)
{
    bool success = true;
    for (IBasePlugin *plugin : m_plugins.values()) {
        if (!plugin->eventManager()->publish(eventName, eventData)) {
            success = false;
        }
    }
    return success;
}

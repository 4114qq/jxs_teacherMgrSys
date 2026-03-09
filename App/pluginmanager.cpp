#include "pluginmanager.h"
#include "../common/interfaces/IConfigManager.h"
#include <QDir>
#include <QDebug>
#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

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

    // 首先收集所有插件信息
    QMap<QString, QString> pluginPathMap;
    for (const QString &fileName : pluginsDir.entryList(QDir::Files)) {
        if (QLibrary::isLibrary(fileName)) {
            QString pluginPath = pluginsDir.absoluteFilePath(fileName);
            
            // 尝试加载插件检查名称
            QPluginLoader loader(pluginPath);
            QObject *pluginObject = loader.instance();
            if (pluginObject) {
                IBasePlugin *plugin = qobject_cast<IBasePlugin*>(pluginObject);
                if (plugin) {
                    pluginPathMap[plugin->name()] = pluginPath;
                    qDebug() << "Found plugin:" << plugin->name() << "at" << pluginPath;
                }
            }
            // 卸载loader以便下次可以重新加载
            loader.unload();
        }
    }

    // 构建依赖关系
    for (const QString &pluginName : pluginPathMap.keys()) {
        // 这里可以从插件接口获取依赖关系
        // 暂时使用配置文件中的依赖
    }

    // 按优先级和依赖关系排序
    QStringList sortedPlugins = sortPluginsByPriority();

    bool success = true;
    for (const QString &pluginName : sortedPlugins) {
        if (pluginPathMap.contains(pluginName)) {
            if (!loadPlugin(pluginPathMap[pluginName])) {
                success = false;
            }
        }
    }

    // 加载未在排序列表中的插件
    for (const QString &pluginName : pluginPathMap.keys()) {
        if (!m_plugins.contains(pluginName)) {
            if (!loadPlugin(pluginPathMap[pluginName])) {
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

    QString name = plugin->name();
    m_mapPluginLoaders[name] = loader;
    m_plugins[name] = plugin;

    IPluginWidget *widget = plugin->pluginWidget();
    if (widget && widget->widget()) {
        emit pluginWidgetReady(name, widget->widget());
    }

    qInfo() << "Plugin loaded successfully:" << name;
    emit pluginLoaded(name);

    if (name == "BaseCorePlugin") {
        IConfigManager *configMgr = plugin->configManager();
        if (configMgr) {
            emit configManagerReady(configMgr);
        }
    }

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

void PluginManager::setPriorityFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open priority file:" << filePath;
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse priority file:" << error.errorString();
        return;
    }

    if (!doc.isObject()) {
        qWarning() << "Priority file is not a JSON object";
        return;
    }

    QJsonObject obj = doc.object();

    // 解析优先级
    if (obj.contains("priority") && obj["priority"].isArray()) {
        QJsonArray priorityArray = obj["priority"].toArray();
        for (int i = 0; i < priorityArray.size(); ++i) {
            QString pluginName = priorityArray[i].toString();
            m_pluginPriorities[pluginName] = i; // 索引越小优先级越高
        }
    }

    // 解析依赖关系
    if (obj.contains("dependencies") && obj["dependencies"].isObject()) {
        QJsonObject dependenciesObj = obj["dependencies"].toObject();
        for (auto it = dependenciesObj.constBegin(); it != dependenciesObj.constEnd(); ++it) {
            QString pluginName = it.key();
            QJsonArray depsArray = it.value().toArray();
            QStringList dependencies;
            for (const QJsonValue &dep : depsArray) {
                dependencies << dep.toString();
            }
            m_pluginDependencies[pluginName] = dependencies;
        }
    }

    qInfo() << "Priority file loaded successfully:" << filePath;
}

void PluginManager::setDefaultPriority(const QString &pluginName, int priority)
{
    m_pluginPriorities[pluginName] = priority;
}

QStringList PluginManager::topologicalSort(const QMap<QString, QStringList> &dependencies)
{
    QStringList result;
    QMap<QString, int> degree;
    QMap<QString, QStringList> adj = dependencies;
    QList<QString> keys = adj.keys();
    for (int i = 0; i < keys.size(); i++) {
        degree[keys.at(i)] = 0;
    }
    QList<QString> allDeps;
    for (int i = 0; i < keys.size(); i++) {
        QStringList d = adj.value(keys.at(i));
        for (int j = 0; j < d.size(); j++) {
            if (!allDeps.contains(d.at(j))) {
                allDeps.append(d.at(j));
            }
            degree[d.at(j)] = degree.value(d.at(j), 0) + 1;
        }
    }
    QList<QString> q;
    QList<QString> k = degree.keys();
    for (int i = 0; i < k.size(); i++) {
        if (degree.value(k.at(i)) == 0) {
            q.append(k.at(i));
        }
    }
    while (!q.isEmpty()) {
        QString cur = q.takeAt(0);
        result.append(cur);
        QStringList d = adj.value(cur);
        for (int i = 0; i < d.size(); i++) {
            QString neighbor = d.at(i);
            degree[neighbor] = degree.value(neighbor) - 1;
            if (degree.value(neighbor) == 0) {
                q.append(neighbor);
            }
        }
    }
    if (result.size() != degree.size()) {
        qWarning() << "Circular dependencies detected!";
    }
    return result;
}

QStringList PluginManager::sortPluginsByPriority()
{
    QStringList sortedByDependency = topologicalSort(m_pluginDependencies);

    std::sort(sortedByDependency.begin(), sortedByDependency.end(), [this](const QString &a, const QString &b) {
        int priorityA = m_pluginPriorities.value(a, 9999);
        int priorityB = m_pluginPriorities.value(b, 9999);
        return priorityA < priorityB;
    });

    return sortedByDependency;
}

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include <QPluginLoader>
#include "../common/interfaces/IBasePlugin.h"

/**
 * @brief 插件管理器
 * 负责加载、管理和卸载插件
 */
class PluginManager : public QObject
{
    Q_OBJECT

public:
    explicit PluginManager(QObject *parent = nullptr);
    ~PluginManager();

    // 插件管理
    bool loadPlugins(const QString &pluginsPath);
    bool loadPlugin(const QString &pluginPath);
    bool unloadPlugin(const QString &pluginName);
    void unloadAllPlugins();

    // 插件查询
    IBasePlugin *getPlugin(const QString &pluginName) const;
    QList<IBasePlugin *> getAllPlugins() const;
    QStringList getPluginNames() const;

    // 事件管理
    bool triggerEvent(const QString &eventName, const QVariantMap &eventData);

public:
    // 优先级管理
    void setPriorityFile(const QString &filePath);
    void setDefaultPriority(const QString &pluginName, int priority);

signals:
    // 插件事件信号
    void pluginLoaded(const QString &pluginName);
    void pluginUnloaded(const QString &pluginName);
    void pluginError(const QString &pluginName, const QString &error);
    // 插件窗口信号
    void pluginWidgetReady(const QString &pluginName, QWidget *widget);



private:
    QMap<QString, QPluginLoader *> m_mapPluginLoaders;
    QMap<QString, IBasePlugin *> m_plugins;
    QMap<QString, int> m_pluginPriorities;         // 插件优先级
    QMap<QString, QStringList> m_pluginDependencies; // 插件依赖
    
    // 辅助方法
    QStringList topologicalSort(const QMap<QString, QStringList> &dependencies);
    QStringList sortPluginsByPriority();
};

#endif // PLUGINMANAGER_H

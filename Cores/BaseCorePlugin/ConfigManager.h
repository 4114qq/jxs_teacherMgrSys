#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "BaseCorePlugin_global.h"
#include "../../common/interfaces/IConfigManager.h"
#include "../../common/interfaces/ILogManager.h"
#include <QObject>
#include <QVariantMap>
#include <QTimer>
#include <QMutex>
#include <QDir>
#include <QStandardPaths>
#include <QJsonObject>

class BASECOREPLUGIN_EXPORT ConfigManager : public QObject, public IConfigManager
{
    Q_OBJECT
    Q_INTERFACES(IConfigManager)

public:
    explicit ConfigManager(QObject *parent = nullptr);
    ~ConfigManager();

    QVariant get(const QString &key, const QVariant &defaultValue = QVariant()) const override;
    void set(const QString &key, const QVariant &value) override;

    void setMultiple(const QVariantMap &map) override;
    QVariantMap getAll() const override;

    bool contains(const QString &key) const override;
    void remove(const QString &key) override;
    void clear() override;

    bool load(const QString &filePath = QString()) override;
    bool save(const QString &filePath = QString()) override;

    bool reload() override;
    void setHotReloadEnabled(bool enabled) override;
    bool isHotReloadEnabled() const override;

    void watch(const QString &key, std::function<void(const QVariant&)> callback) override;
    void unwatch(const QString &key) override;

    void beginGroup(const QString &group) override;
    void endGroup() override;
    QString currentGroup() const override;

    QStringList allKeys() const override;
    QString getConfigFilePath() const override;

    void setConfigFileName(const QString &fileName);
    QString configFileName() const;

    QList<ConfigItem> getConfigItems() const override;
    void addConfigItem(const ConfigItem &item) override;
    void updateConfigItem(const ConfigItem &item) override;
    void deleteConfigItem(const QString &key) override;

signals:
    void configChanged(const QString &key, const QVariant &value);
    void configReloaded();

public:
    void setLogManager(ILogManager *manager);
    QString fullKey(const QString &key) const;
    void notifyWatchers(const QString &key, const QVariant &value);
    QString getDefaultConfigPath() const;
    static QJsonObject jsonObjectFromVariantMap(const QVariantMap &map);
    static QVariantMap variantMapFromJsonObject(const QJsonObject &obj);

private:
    QList<ConfigItem> m_configItems;
    mutable QString m_configPath;
    QString m_fileName;
    QStringList m_groupStack;
    bool m_hotReloadEnabled;
    QTimer *m_reloadTimer;

    mutable QMutex m_mutex;
    ILogManager *m_logManager;
    QMap<QString, std::function<void(const QVariant&)>> m_watchers;
};

#endif // CONFIGMANAGER_H

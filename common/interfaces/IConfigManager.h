#ifndef ICONFIGMANAGER_H
#define ICONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QList>
#include <functional>

struct ConfigItem {
    QString key;
    QVariant value;
    QString description;
    QString type;
};

class IConfigManager
{
public:
    virtual ~IConfigManager() = default;

    virtual QVariant get(const QString &key, const QVariant &defaultValue = QVariant()) const = 0;
    virtual void set(const QString &key, const QVariant &value) = 0;

    virtual void setMultiple(const QVariantMap &map) = 0;
    virtual QVariantMap getAll() const = 0;

    virtual bool contains(const QString &key) const = 0;
    virtual void remove(const QString &key) = 0;
    virtual void clear() = 0;

    virtual bool load(const QString &filePath = QString()) = 0;
    virtual bool save(const QString &filePath = QString()) = 0;

    virtual bool reload() = 0;
    virtual void setHotReloadEnabled(bool enabled) = 0;
    virtual bool isHotReloadEnabled() const = 0;

    virtual void watch(const QString &key, std::function<void(const QVariant&)> callback) = 0;
    virtual void unwatch(const QString &key) = 0;

    virtual void beginGroup(const QString &group) = 0;
    virtual void endGroup() = 0;
    virtual QString currentGroup() const = 0;

    virtual QStringList allKeys() const = 0;
    virtual QString getConfigFilePath() const = 0;

    virtual QList<ConfigItem> getConfigItems() const = 0;
    virtual void addConfigItem(const ConfigItem &item) = 0;
    virtual void updateConfigItem(const ConfigItem &item) = 0;
    virtual void deleteConfigItem(const QString &key) = 0;
};

#define ICONFIGMANAGER_IID "com.jxs.teacherMgrSys.IConfigManager"
Q_DECLARE_INTERFACE(IConfigManager, ICONFIGMANAGER_IID)

#endif // ICONFIGMANAGER_H

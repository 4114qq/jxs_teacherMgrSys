#include "ConfigManager.h"
#include "../../common/interfaces/ILogManager.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMutexLocker>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_fileName("config.json")
    , m_hotReloadEnabled(false)
    , m_reloadTimer(new QTimer(this))
    , m_logManager(nullptr)
{
    connect(m_reloadTimer, &QTimer::timeout, this, &ConfigManager::reload);
}

ConfigManager::~ConfigManager()
{
    if (m_reloadTimer->isActive()) {
        m_reloadTimer->stop();
    }
}

QString ConfigManager::getDefaultConfigPath() const
{
    return QCoreApplication::applicationDirPath() + "/configs/" + m_fileName;
}

QVariant ConfigManager::get(const QString &key, const QVariant &defaultValue) const
{
    QMutexLocker locker(&m_mutex);

    for (const auto &item : m_configItems) {
        if (item.key == key) {
            return item.value;
        }
    }
    return defaultValue;
}

void ConfigManager::set(const QString &key, const QVariant &value)
{
    QMutexLocker locker(&m_mutex);

    for (auto &item : m_configItems) {
        if (item.key == key) {
            item.value = value.toString();
            notifyWatchers(key, value);
            emit configChanged(key, value);
            return;
        }
    }
}

void ConfigManager::setMultiple(const QVariantMap &map)
{
    QMutexLocker locker(&m_mutex);

    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        for (auto &item : m_configItems) {
            if (item.key == it.key()) {
                item.value = it.value().toString();
                notifyWatchers(it.key(), it.value());
                emit configChanged(it.key(), it.value());
                break;
            }
        }
    }
}

QVariantMap ConfigManager::getAll() const
{
    QMutexLocker locker(&m_mutex);
    QVariantMap map;
    for (const auto &item : m_configItems) {
        map[item.key] = item.value;
    }
    return map;
}

bool ConfigManager::contains(const QString &key) const
{
    QMutexLocker locker(&m_mutex);

    for (const auto &item : m_configItems) {
        if (item.key == key) {
            return true;
        }
    }
    return false;
}

void ConfigManager::remove(const QString &key)
{
    QMutexLocker locker(&m_mutex);

    for (int i = 0; i < m_configItems.size(); ++i) {
        if (m_configItems[i].key == key) {
            m_configItems.removeAt(i);
            m_watchers.remove(key);
            return;
        }
    }
}

void ConfigManager::clear()
{
    QMutexLocker locker(&m_mutex);
    m_configItems.clear();
    m_watchers.clear();
}

bool ConfigManager::load(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

    QString path = filePath.isEmpty() ? getDefaultConfigPath() : filePath;
    m_configPath = path;

    QFile file(path);
    if (!file.exists()) {
        if (m_logManager) {
            m_logManager->logDebug("ConfigManager", QString("Config file does not exist, using default config: %1").arg(path));
        }
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        if (m_logManager) {
            m_logManager->logWarning("ConfigManager", QString("Failed to open config file: %1, %2").arg(path, file.errorString()));
        }
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        if (m_logManager) {
            m_logManager->logWarning("ConfigManager", QString("Failed to parse config file: %1").arg(error.errorString()));
        }
        return false;
    }

    if (!doc.isObject()) {
        if (m_logManager) {
            m_logManager->logWarning("ConfigManager", "Config file is not a JSON object");
        }
        return false;
    }

    QJsonObject obj = doc.object();
    m_configItems.clear();

    QStringList keys = obj.keys();
    for (const QString &fullKey : keys) {
        if (fullKey.endsWith("_note")) {
            continue;
        }

        QJsonValue value = obj[fullKey];
        QString noteKey = fullKey + "_note";
        QString description = obj.contains(noteKey) ? obj[noteKey].toString() : QString();

        ConfigItem item;
        item.key = fullKey;
        item.description = description;

        if (value.isString()) {
            item.value = value.toString();
            item.type = "string";
        } else if (value.isBool()) {
            item.value = value.toBool();
            item.type = "bool";
        } else if (value.isDouble()) {
            double doubleValue = value.toDouble();
            if (doubleValue == static_cast<int>(doubleValue)) {
                item.value = static_cast<int>(doubleValue);
                item.type = "int";
            } else {
                item.value = doubleValue;
                item.type = "double";
            }
        } else {
            item.value = value.toVariant();
            item.type = "string";
        }

        m_configItems.append(item);
    }

    if (m_logManager) {
        m_logManager->logInfo("ConfigManager", QString("Config loaded from: %1 with %2 items").arg(path).arg(m_configItems.size()));
    }
    return true;
}

bool ConfigManager::save(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

    if (m_configItems.isEmpty()) {
        if (m_logManager) {
            m_logManager->logDebug("ConfigManager", "No config items to save, skipping save operation");
        }
        return true;
    }

    QString path = filePath.isEmpty() ? getDefaultConfigPath() : filePath;
    m_configPath = path;

    QFileInfo fileInfo(path);
    QDir dir = fileInfo.dir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        if (m_logManager) {
            m_logManager->logError("ConfigManager", QString("Failed to open config file for writing: %1, %2").arg(path, file.errorString()));
        }
        return false;
    }

    QJsonObject rootObj;
    for (const auto &item : m_configItems) {
        QJsonValue value;
        if (item.type == "bool") {
            value = item.value.toBool();
        } else if (item.type == "int") {
            value = item.value.toInt();
        } else if (item.type == "double") {
            value = item.value.toDouble();
        } else {
            value = item.value.toString();
        }
        rootObj[item.key] = value;

        if (!item.description.isEmpty()) {
            rootObj[item.key + "_note"] = item.description;
        }
    }

    QJsonDocument doc(rootObj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    if (m_logManager) {
        m_logManager->logInfo("ConfigManager", QString("Config saved to: %1").arg(path));
    }
    return true;
}

bool ConfigManager::reload()
{
    QString path;
    {
        QMutexLocker locker(&m_mutex);
        path = m_configPath.isEmpty() ? getDefaultConfigPath() : m_configPath;
    }

    if (!load(path)) {
        return false;
    }

    emit configReloaded();
    return true;
}

void ConfigManager::setHotReloadEnabled(bool enabled)
{
    QMutexLocker locker(&m_mutex);

    m_hotReloadEnabled = enabled;
    if (enabled) {
        if (!m_reloadTimer->isActive()) {
            m_reloadTimer->start(5000);
        }
    } else {
        m_reloadTimer->stop();
    }
}

bool ConfigManager::isHotReloadEnabled() const
{
    QMutexLocker locker(&m_mutex);
    return m_hotReloadEnabled;
}

void ConfigManager::watch(const QString &key, std::function<void(const QVariant&)> callback)
{
    QMutexLocker locker(&m_mutex);
    m_watchers[key] = callback;
}

void ConfigManager::unwatch(const QString &key)
{
    QMutexLocker locker(&m_mutex);
    m_watchers.remove(key);
}

void ConfigManager::beginGroup(const QString &group)
{
    QMutexLocker locker(&m_mutex);
    m_groupStack.append(group);
}

void ConfigManager::endGroup()
{
    QMutexLocker locker(&m_mutex);
    if (!m_groupStack.isEmpty()) {
        m_groupStack.removeLast();
    }
}

QString ConfigManager::currentGroup() const
{
    QMutexLocker locker(&m_mutex);
    return m_groupStack.join("/");
}

QStringList ConfigManager::allKeys() const
{
    QMutexLocker locker(&m_mutex);
    QStringList keys;
    for (const auto &item : m_configItems) {
        keys.append(item.key);
    }
    return keys;
}

QString ConfigManager::getConfigFilePath() const
{
    QMutexLocker locker(&m_mutex);
    return m_configPath.isEmpty() ? getDefaultConfigPath() : m_configPath;
}

void ConfigManager::setConfigFileName(const QString &fileName)
{
    QMutexLocker locker(&m_mutex);
    m_fileName = fileName;
}

QString ConfigManager::configFileName() const
{
    QMutexLocker locker(&m_mutex);
    return m_fileName;
}

QString ConfigManager::fullKey(const QString &key) const
{
    if (m_groupStack.isEmpty()) {
        return key;
    }
    return m_groupStack.join("/") + "/" + key;
}

void ConfigManager::notifyWatchers(const QString &key, const QVariant &value)
{
    if (m_watchers.contains(key)) {
        m_watchers[key](value);
    }
}

QJsonObject ConfigManager::jsonObjectFromVariantMap(const QVariantMap &map)
{
    QJsonObject obj;
    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        obj[it.key()] = QJsonValue::fromVariant(it.value());
    }
    return obj;
}

QVariantMap ConfigManager::variantMapFromJsonObject(const QJsonObject &obj)
{
    QVariantMap map;
    for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
        map[it.key()] = it.value().toVariant();
    }
    return map;
}

void ConfigManager::setLogManager(ILogManager *manager)
{
    m_logManager = manager;
}

QList<ConfigItem> ConfigManager::getConfigItems() const
{
    QMutexLocker locker(&m_mutex);
    return m_configItems;
}

void ConfigManager::addConfigItem(const ConfigItem &item)
{
    QMutexLocker locker(&m_mutex);
    m_configItems.append(item);
}

void ConfigManager::updateConfigItem(const ConfigItem &item)
{
    QMutexLocker locker(&m_mutex);

    for (auto &existingItem : m_configItems) {
        if (existingItem.key == item.key) {
            existingItem = item;
            emit configChanged(item.key, item.value);
            return;
        }
    }
}

void ConfigManager::deleteConfigItem(const QString &key)
{
    QMutexLocker locker(&m_mutex);

    for (int i = 0; i < m_configItems.size(); ++i) {
        if (m_configItems[i].key == key) {
            m_configItems.removeAt(i);
            m_watchers.remove(key);
            return;
        }
    }
}

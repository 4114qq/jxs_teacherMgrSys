/**
 * @file ConfigManager.cpp
 * @brief 配置管理器实现
 * @details 提供配置的读取、写入、保存、热重载等功能实现
 */

#include "ConfigManager.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMutexLocker>

// ===== 构造函数 =====

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

// ===== 基本读写 =====

QString ConfigManager::getDefaultConfigPath() const
{
    return QCoreApplication::applicationDirPath() + "/configs/" + m_fileName;
}

QVariant ConfigManager::get(const QString &key, const QVariant &defaultValue) const
{
    QMutexLocker locker(&m_mutex);

    QString group, actualKey;
    if (key.contains('.')) {
        int pos = key.lastIndexOf('.');
        group = key.mid(0, pos);
        actualKey = key.mid(pos + 1);
    } else {
        actualKey = key;
    }

    for (const auto &item : m_configItems) {
        if (item.key == actualKey && item.group == group) {
            return item.value;
        }
    }
    return defaultValue;
}

void ConfigManager::set(const QString &key, const QVariant &value)
{
    QMutexLocker locker(&m_mutex);

    QString group, actualKey;
    if (key.contains('.')) {
        int pos = key.lastIndexOf('.');
        group = key.mid(0, pos);
        actualKey = key.mid(pos + 1);
    } else {
        actualKey = key;
    }

    for (auto &item : m_configItems) {
        if (item.key == actualKey && item.group == group) {
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
        QString key = it.key();
        QString group, actualKey;
        if (key.contains('.')) {
            int pos = key.lastIndexOf('.');
            group = key.mid(0, pos);
            actualKey = key.mid(pos + 1);
        } else {
            actualKey = key;
        }

        for (auto &item : m_configItems) {
            if (item.key == actualKey && item.group == group) {
                item.value = it.value().toString();
                notifyWatchers(key, it.value());
                emit configChanged(key, it.value());
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

// ===== 配置检查 =====

bool ConfigManager::contains(const QString &key) const
{
    QMutexLocker locker(&m_mutex);

    QString group, actualKey;
    if (key.contains('.')) {
        int pos = key.lastIndexOf('.');
        group = key.mid(0, pos);
        actualKey = key.mid(pos + 1);
    } else {
        actualKey = key;
    }

    for (const auto &item : m_configItems) {
        if (item.key == actualKey && item.group == group) {
            return true;
        }
    }
    return false;
}

void ConfigManager::remove(const QString &key)
{
    QMutexLocker locker(&m_mutex);

    QString group, actualKey;
    if (key.contains('.')) {
        int pos = key.lastIndexOf('.');
        group = key.mid(0, pos);
        actualKey = key.mid(pos + 1);
    } else {
        actualKey = key;
    }

    for (int i = 0; i < m_configItems.size(); ++i) {
        if (m_configItems[i].key == actualKey && m_configItems[i].group == group) {
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

// ===== 配置持久化 =====

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

    parseJsonObject(obj, QString());

    if (m_logManager) {
        m_logManager->logInfo("ConfigManager", QString("Config loaded from: %1 with %2 items").arg(path).arg(m_configItems.size()));
    }
    return true;
}

void ConfigManager::parseJsonObject(const QJsonObject &obj, const QString &group)
{
    QStringList keys = obj.keys();
    for (const QString &fullKey : keys) {
        if (fullKey.endsWith("_note")) {
            continue;
        }

        QJsonValue value = obj[fullKey];
        QString noteKey = fullKey + "_note";
        QString description = obj.contains(noteKey) ? obj[noteKey].toString() : QString();

        if (value.isObject()) {
            QString newGroup = group.isEmpty() ? fullKey : group + "." + fullKey;
            parseJsonObject(value.toObject(), newGroup);
            continue;
        }

        ConfigItem item;
        item.key = fullKey;
        item.group = group;
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

    if (group.isEmpty()) {
        return;
    }

    QString groupNoteKey = group.split(".").last() + "_note";
    if (obj.contains(groupNoteKey)) {
        QString groupDescription = obj[groupNoteKey].toString();
        for (auto &item : m_configItems) {
            if (item.group == group && item.description.isEmpty()) {
                item.description = groupDescription;
            }
        }
    }
}

bool ConfigManager::save(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

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

    QJsonObject rootObj = buildJsonObject();

    QJsonDocument doc(rootObj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    if (m_logManager) {
        m_logManager->logInfo("ConfigManager", QString("Config saved to: %1").arg(path));
    }
    return true;
}

QJsonObject ConfigManager::buildJsonObject() const
{
    QJsonObject rootObj;
    QMap<QString, QJsonObject> groups;

    for (const auto &item : m_configItems) {
        if (item.key.isEmpty()) {
            continue;
        }

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

        if (item.group.isEmpty()) {
            rootObj[item.key] = value;
            if (!item.description.isEmpty()) {
                rootObj[item.key + "_note"] = item.description;
            }
        } else {
            if (!groups.contains(item.group)) {
                groups[item.group] = QJsonObject();
            }

            QJsonObject groupObj = groups[item.group];
            groupObj[item.key] = value;
            if (!item.description.isEmpty()) {
                groupObj[item.key + "_note"] = item.description;
            }
            groups[item.group] = groupObj;
        }
    }

    for (auto it = groups.constBegin(); it != groups.constEnd(); ++it) {
        QString groupName = it.key();
        QJsonObject groupData = it.value();

        QStringList parts = groupName.split(".");
        QJsonObject nestedObj = groupData;

        for (int i = parts.size() - 1; i >= 0; --i) {
            QJsonObject wrapper;
            wrapper[parts[i]] = nestedObj;
            nestedObj = wrapper;
        }

        for (auto key : nestedObj.keys()) {
            if (!rootObj.contains(key)) {
                rootObj[key] = nestedObj[key];
            } else {
                QJsonObject existing = rootObj[key].toObject();
                QJsonObject toMerge = nestedObj[key].toObject();
                for (auto mergeKey : toMerge.keys()) {
                    existing[mergeKey] = toMerge[mergeKey];
                }
                rootObj[key] = existing;
            }
        }
    }

    return rootObj;
}

// ===== 热重载 =====

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

// ===== 配置监视 =====

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

void ConfigManager::notifyWatchers(const QString &key, const QVariant &value)
{
    if (m_watchers.contains(key)) {
        m_watchers[key](value);
    }
}

// ===== 分组管理 =====

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

QString ConfigManager::fullKey(const QString &key) const
{
    if (m_groupStack.isEmpty()) {
        return key;
    }
    return m_groupStack.join("/") + "/" + key;
}

// ===== 工具方法 =====

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

// ===== 配置项管理 =====

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

    if (item.key.isEmpty()) {
        return;
    }

    QString group = item.group;
    QString actualKey = item.key;

    for (auto &existingItem : m_configItems) {
        if (existingItem.key == actualKey && existingItem.group == group) {
            existingItem.value = item.value;
            existingItem.description = item.description;
            if (!item.type.isEmpty()) {
                existingItem.type = item.type;
            }
            QString fullKey = group.isEmpty() ? actualKey : group + "." + actualKey;
            emit configChanged(fullKey, existingItem.value);
            return;
        }
    }

    m_configItems.append(item);
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

#include "ConfigManager.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QMutexLocker>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_fileName("config.json")
    , m_hotReloadEnabled(false)
    , m_reloadTimer(new QTimer(this))
{
    connect(m_reloadTimer, &QTimer::timeout, this, &ConfigManager::reload);
}

ConfigManager::~ConfigManager()
{
    if (m_reloadTimer->isActive()) {
        m_reloadTimer->stop();
    }
    save();
}

QString ConfigManager::getDefaultConfigPath() const
{
#ifdef Q_OS_WIN
    return QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
           .filePath(m_fileName);
#elif defined(Q_OS_LINUX)
    return QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation))
           .filePath(QString("jxs_teacherMgrSys/%1").arg(m_fileName));
#else
    return QCoreApplication::applicationDirPath() + "/" + m_fileName;
#endif
}

QVariant ConfigManager::get(const QString &key, const QVariant &defaultValue) const
{
    QMutexLocker locker(&m_mutex);

    QString full = fullKey(key);
    if (m_data.contains(full)) {
        return m_data[full];
    }
    return defaultValue;
}

void ConfigManager::set(const QString &key, const QVariant &value)
{
    QMutexLocker locker(&m_mutex);

    QString full = fullKey(key);
    m_data[full] = value;
    notifyWatchers(full, value);
    emit configChanged(full, value);
}

void ConfigManager::setMultiple(const QVariantMap &map)
{
    QMutexLocker locker(&m_mutex);

    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        QString full = fullKey(it.key());
        m_data[full] = it.value();
        notifyWatchers(full, it.value());
        emit configChanged(full, it.value());
    }
}

QVariantMap ConfigManager::getAll() const
{
    QMutexLocker locker(&m_mutex);
    return m_data;
}

bool ConfigManager::contains(const QString &key) const
{
    QMutexLocker locker(&m_mutex);
    return m_data.contains(fullKey(key));
}

void ConfigManager::remove(const QString &key)
{
    QMutexLocker locker(&m_mutex);

    QString full = fullKey(key);
    m_data.remove(full);
    m_watchers.remove(full);
}

void ConfigManager::clear()
{
    QMutexLocker locker(&m_mutex);
    m_data.clear();
    m_watchers.clear();
}

bool ConfigManager::load(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);

    QString path = filePath.isEmpty() ? getDefaultConfigPath() : filePath;
    m_configPath = path;

    QFile file(path);
    if (!file.exists()) {
        qDebug() << "Config file does not exist, using default config:" << path;
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file:" << path << file.errorString();
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse config file:" << error.errorString();
        return false;
    }

    if (!doc.isObject()) {
        qWarning() << "Config file is not a JSON object";
        return false;
    }

    QJsonObject obj = doc.object();
    m_data = variantMapFromJsonObject(obj);

    qDebug() << "Config loaded from:" << path;
    return true;
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
        qWarning() << "Failed to open config file for writing:" << path << file.errorString();
        return false;
    }

    QJsonObject obj = jsonObjectFromVariantMap(m_data);
    QJsonDocument doc(obj);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();

    qDebug() << "Config saved to:" << path;
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
    m_watchers[fullKey(key)] = callback;
}

void ConfigManager::unwatch(const QString &key)
{
    QMutexLocker locker(&m_mutex);
    m_watchers.remove(fullKey(key));
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
    return m_data.keys();
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

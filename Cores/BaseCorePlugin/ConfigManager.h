/**
 * @file ConfigManager.h
 * @brief 配置管理器实现
 * @details 提供配置的读取、写入、保存、热重载等功能
 */

#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "BaseCorePlugin_global.h"
#include "../../common/interfaces/IConfigManager.h"
#include "interfaces/ILogManager.h"
#include <QObject>
#include <QVariantMap>
#include <QTimer>
#include <QMutex>
#include <QDir>
#include <QStandardPaths>
#include <QJsonObject>

/**
 * @class ConfigManager
 * @brief 配置管理器
 * @details 实现配置的持久化、监视、热重载等功能
 */
class BASECOREPLUGIN_EXPORT ConfigManager : public QObject, public IConfigManager
{
    Q_OBJECT
    Q_INTERFACES(IConfigManager)

public:
    // ===== 构造函数 =====

    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit ConfigManager(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~ConfigManager();

    // ===== 基本读写 =====

    /**
     * @brief 获取配置值
     * @param key 配置键名
     * @param defaultValue 默认值
     * @return QVariant 配置值
     */
    QVariant get(const QString &key, const QVariant &defaultValue = QVariant()) const override;

    /**
     * @brief 设置配置值
     * @param key 配置键名
     * @param value 配置值
     */
    void set(const QString &key, const QVariant &value) override;

    /**
     * @brief 批量设置配置
     * @param map 配置键值对映射
     */
    void setMultiple(const QVariantMap &map) override;

    /**
     * @brief 获取所有配置
     * @return QVariantMap 所有配置键值对
     */
    QVariantMap getAll() const override;

    // ===== 配置检查 =====

    /**
     * @brief 检查配置是否存在
     * @param key 配置键名
     * @return bool 是否存在
     */
    bool contains(const QString &key) const override;

    /**
     * @brief 移除配置
     * @param key 配置键名
     */
    void remove(const QString &key) override;

    /**
     * @brief 清除所有配置
     */
    void clear() override;

    // ===== 配置持久化 =====

    /**
     * @brief 加载配置文件
     * @param filePath 文件路径（为空时使用默认路径）
     * @return bool 加载是否成功
     */
    bool load(const QString &filePath = QString()) override;

    /**
     * @brief 保存配置文件
     * @param filePath 文件路径（为空时使用默认路径）
     * @return bool 保存是否成功
     */
    bool save(const QString &filePath = QString()) override;

    // ===== 热重载 =====

    /**
     * @brief 重新加载配置
     * @return bool 重新加载是否成功
     */
    bool reload() override;

    /**
     * @brief 设置热重载是否启用
     * @param enabled 是否启用
     */
    void setHotReloadEnabled(bool enabled) override;

    /**
     * @brief 检查热重载是否启用
     * @return bool 是否启用
     */
    bool isHotReloadEnabled() const override;

    // ===== 配置监视 =====

    /**
     * @brief 监视配置变化
     * @param key 配置键名
     * @param callback 回调函数
     */
    void watch(const QString &key, std::function<void(const QVariant&)> callback) override;

    /**
     * @brief 取消监视配置
     * @param key 配置键名
     */
    void unwatch(const QString &key) override;

    // ===== 分组管理 =====

    /**
     * @brief 开始分组
     * @param group 分组名称
     */
    void beginGroup(const QString &group) override;

    /**
     * @brief 结束分组
     */
    void endGroup() override;

    /**
     * @brief 获取当前分组
     * @return QString 当前分组名称
     */
    QString currentGroup() const override;

    // ===== 工具方法 =====

    /**
     * @brief 获取所有配置键
     * @return QStringList 配置键列表
     */
    QStringList allKeys() const override;

    /**
     * @brief 获取配置文件路径
     * @return QString 配置文件路径
     */
    QString getConfigFilePath() const override;

    /**
     * @brief 设置配置文件名
     * @param fileName 文件名
     */
    void setConfigFileName(const QString &fileName);

    /**
     * @brief 获取配置文件名
     * @return QString 文件名
     */
    QString configFileName() const;

    // ===== 配置项管理 =====

    /**
     * @brief 获取配置项列表
     * @return QList<ConfigItem> 配置项列表
     */
    QList<ConfigItem> getConfigItems() const override;

    /**
     * @brief 添加配置项
     * @param item 配置项
     */
    void addConfigItem(const ConfigItem &item) override;

    /**
     * @brief 更新配置项
     * @param item 配置项
     */
    void updateConfigItem(const ConfigItem &item) override;

    /**
     * @brief 删除配置项
     * @param key 配置键名
     */
    void deleteConfigItem(const QString &key) override;

signals:
    /**
     * @brief 配置变化信号
     * @param key 配置键名
     * @param value 配置值
     */
    void configChanged(const QString &key, const QVariant &value);

    /**
     * @brief 配置重新加载信号
     */
    void configReloaded();

public:
    /**
     * @brief 设置日志管理器
     * @param manager 日志管理器指针
     */
    void setLogManager(ILogManager *manager);

    /**
     * @brief 获取完整键名
     * @param key 键名
     * @return QString 完整键名
     */
    QString fullKey(const QString &key) const;

    /**
     * @brief 通知监视器
     * @param key 配置键名
     * @param value 配置值
     */
    void notifyWatchers(const QString &key, const QVariant &value);

    /**
     * @brief 解析JSON对象
     * @param obj JSON对象
     * @param group 分组名称
     */
    void parseJsonObject(const QJsonObject &obj, const QString &group);

    /**
     * @brief 构建JSON对象
     * @return QJsonObject JSON对象
     */
    QJsonObject buildJsonObject() const;

    /**
     * @brief 获取默认配置路径
     * @return QString 默认配置路径
     */
    QString getDefaultConfigPath() const;

    /**
     * @brief 从VariantMap转换为JSON对象
     * @param map VariantMap
     * @return QJsonObject JSON对象
     */
    static QJsonObject jsonObjectFromVariantMap(const QVariantMap &map);

    /**
     * @brief 从JSON对象转换为VariantMap
     * @param obj JSON对象
     * @return QVariantMap VariantMap
     */
    static QVariantMap variantMapFromJsonObject(const QJsonObject &obj);

private:
    QList<ConfigItem> m_configItems;                              ///< 配置项列表
    mutable QString m_configPath;                                ///< 配置文件路径
    QString m_fileName;                                          ///< 文件名
    QStringList m_groupStack;                                    ///< 分组栈
    bool m_hotReloadEnabled;                                    ///< 热重载是否启用
    QTimer *m_reloadTimer;                                      ///< 重载定时器

    mutable QMutex m_mutex;                                     ///< 互斥锁
    ILogManager *m_logManager;                                  ///< 日志管理器
    QMap<QString, std::function<void(const QVariant&)>> m_watchers; ///< 配置监视器
};

#endif // CONFIGMANAGER_H

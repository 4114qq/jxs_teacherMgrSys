/**
 * @file IConfigManager.h
 * @brief 配置管理接口定义
 * @details 定义配置读写、热重载、监视等功能的接口
 */

#ifndef ICONFIGMANAGER_H
#define ICONFIGMANAGER_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QList>
#include <functional>

/**
 * @struct ConfigItem
 * @brief 配置项结构体
 */
struct ConfigItem {
    QString key;        ///< 配置键名
    QString group;     ///< 配置分组
    QVariant value;    ///< 配置值
    QString description; ///< 配置描述
    QString type;      ///< 配置类型
};

/**
 * @class IConfigManager
 * @brief 配置管理器接口
 * @details 提供配置的读取、写入、保存、热重载等功能
 */
class IConfigManager
{
public:
    /**
     * @brief 析构函数
     */
    virtual ~IConfigManager() = default;

    // ===== 基本读写 =====

    /**
     * @brief 获取配置值
     * @param key 配置键名
     * @param defaultValue 默认值
     * @return QVariant 配置值
     */
    virtual QVariant get(const QString &key, const QVariant &defaultValue = QVariant()) const = 0;

    /**
     * @brief 设置配置值
     * @param key 配置键名
     * @param value 配置值
     */
    virtual void set(const QString &key, const QVariant &value) = 0;

    /**
     * @brief 批量设置配置
     * @param map 配置键值对映射
     */
    virtual void setMultiple(const QVariantMap &map) = 0;

    /**
     * @brief 获取所有配置
     * @return QVariantMap 所有配置
     */
    virtual QVariantMap getAll() const = 0;

    // ===== 配置检查 =====

    /**
     * @brief 检查配置是否存在
     * @param key 配置键名
     * @return bool 是否存在
     */
    virtual bool contains(const QString &key) const = 0;

    /**
     * @brief 删除配置
     * @param key 配置键名
     */
    virtual void remove(const QString &key) = 0;

    /**
     * @brief 清除所有配置
     */
    virtual void clear() = 0;

    // ===== 配置持久化 =====

    /**
     * @brief 加载配置
     * @param filePath 配置文件路径（空则使用默认路径）
     * @return bool 加载是否成功
     */
    virtual bool load(const QString &filePath = QString()) = 0;

    /**
     * @brief 保存配置
     * @param filePath 配置文件路径（空则使用默认路径）
     * @return bool 保存是否成功
     */
    virtual bool save(const QString &filePath = QString()) = 0;

    /**
     * @brief 重新加载配置
     * @return bool 重新加载是否成功
     */
    virtual bool reload() = 0;

    // ===== 热重载 =====

    /**
     * @brief 设置热重载是否启用
     * @param enabled 是否启用
     */
    virtual void setHotReloadEnabled(bool enabled) = 0;

    /**
     * @brief 检查热重载是否启用
     * @return bool 是否启用
     */
    virtual bool isHotReloadEnabled() const = 0;

    // ===== 配置监视 =====

    /**
     * @brief 监视配置变化
     * @param key 配置键名
     * @param callback 回调函数
     */
    virtual void watch(const QString &key, std::function<void(const QVariant&)> callback) = 0;

    /**
     * @brief 取消监视
     * @param key 配置键名
     */
    virtual void unwatch(const QString &key) = 0;

    // ===== 分组管理 =====

    /**
     * @brief 开始分组
     * @param group 分组名称
     */
    virtual void beginGroup(const QString &group) = 0;

    /**
     * @brief 结束分组
     */
    virtual void endGroup() = 0;

    /**
     * @brief 获取当前分组
     * @return QString 当前分组名称
     */
    virtual QString currentGroup() const = 0;

    // ===== 工具方法 =====

    /**
     * @brief 获取所有配置键
     * @return QStringList 配置键列表
     */
    virtual QStringList allKeys() const = 0;

    /**
     * @brief 获取配置文件路径
     * @return QString 配置文件路径
     */
    virtual QString getConfigFilePath() const = 0;

    // ===== 配置项管理 =====

    /**
     * @brief 获取所有配置项
     * @return QList<ConfigItem> 配置项列表
     */
    virtual QList<ConfigItem> getConfigItems() const = 0;

    /**
     * @brief 添加配置项
     * @param item 配置项
     */
    virtual void addConfigItem(const ConfigItem &item) = 0;

    /**
     * @brief 更新配置项
     * @param item 配置项
     */
    virtual void updateConfigItem(const ConfigItem &item) = 0;

    /**
     * @brief 删除配置项
     * @param key 配置键名
     */
    virtual void deleteConfigItem(const QString &key) = 0;
};

#define ICONFIGMANAGER_IID "com.jxs.teacherMgrSys.IConfigManager"
Q_DECLARE_INTERFACE(IConfigManager, ICONFIGMANAGER_IID)

#endif // ICONFIGMANAGER_H

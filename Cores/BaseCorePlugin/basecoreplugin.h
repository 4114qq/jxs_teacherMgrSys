/**
 * @file basecoreplugin.h
 * @brief 基础核心插件头文件
 * @details 实现IBasePlugin接口，提供数据库、日志、认证、配置、HTTP等核心服务
 */

#ifndef BASECOREPLUGIN_H
#define BASECOREPLUGIN_H

#include "BaseCorePlugin_global.h"

#include "../../common/interfaces/IBasePlugin.h"
#include "DatabaseManager.h"
#include "ConfigManager.h"
#include "LogManager.h"
#include "AuthManager.h"
#include "HttpClientManager.h"
#include "EventManager.h"
#include "ThemeManager.h"

/**
 * @class BaseCorePlugin
 * @brief 基础核心插件
 * @details 实现了IBasePlugin接口，是整个插件系统的核心
 * 
 * 提供的核心服务：
 * - 事件管理 (EventManager)
 * - 数据库管理 (DatabaseManager)
 * - 配置管理 (ConfigManager)
 * - 日志管理 (LogManager)
 * - 认证管理 (AuthManager)
 * - HTTP客户端管理 (HttpClientManager)
 */
class BASECOREPLUGIN_EXPORT BaseCorePlugin : public QObject, public IBasePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IBASEPLUGIN_IID)
    Q_INTERFACES(IBasePlugin)

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    explicit BaseCorePlugin(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~BaseCorePlugin() override;

    // ===== 插件基本信息 =====

    /**
     * @brief 获取插件名称
     * @return QString 插件名称
     */
    QString name() const override;

    /**
     * @brief 获取插件版本号
     * @return QString 版本号
     */
    QString version() const override;

    /**
     * @brief 获取插件描述
     * @return QString 插件描述
     */
    QString description() const override;

    /**
     * @brief 获取插件作者
     * @return QString 作者名称
     */
    QString author() const override;

    // ===== 核心插件设置 =====

    /**
     * @brief 设置核心插件引用
     * @param core 核心插件指针
     */
    void setCore(IBasePlugin *core) override;

    /**
     * @brief 获取核心插件引用
     * @return IBasePlugin* 核心插件指针
     */
    IBasePlugin *core() const override;

    // ===== 依赖管理 =====

    /**
     * @brief 获取插件依赖列表
     * @return QStringList 依赖列表（基础插件无依赖）
     */
    QStringList dependencies() const override;

    // ===== 核心服务获取 =====

    /**
     * @brief 获取事件总线管理器
     * @return IBaseEventBus* 事件总线管理器指针
     */
    IBaseEventBus *eventManager() const override;

    /**
     * @brief 获取数据库管理器
     * @return IDatabaseManager* 数据库管理器指针
     */
    IDatabaseManager *databaseManager() const override;

    /**
     * @brief 获取配置管理器
     * @return IConfigManager* 配置管理器指针
     */
    IConfigManager *configManager() const override;

    /**
     * @brief 获取日志管理器
     * @return ILogManager* 日志管理器指针
     */
    ILogManager *logManager() const override;

    /**
     * @brief 获取认证管理器
     * @return IAuthManager* 认证管理器指针
     */
    IAuthManager *authManager() const override;

    /**
     * @brief 获取HTTP客户端管理器
     * @return IHttpClientManager* HTTP客户端管理器指针
     */
    IHttpClientManager *httpClientManager() const override;

    /**
     * @brief 获取主题管理器
     * @return IThemeManager* 主题管理器指针
     */
    IThemeManager *themeManager() const override;

    // ===== 插件生命周期管理 =====

    /**
     * @brief 初始化插件
     * @return bool 初始化是否成功
     */
    bool initialize() override;

    /**
     * @brief 启动插件
     * @return bool 启动是否成功
     */
    bool startPlugin() override;

    /**
     * @brief 停止插件
     * @return bool 停止是否成功
     */
    bool stopPlugin() override;

    /**
     * @brief 清理插件资源
     */
    void cleanup() override;

private:
    // 核心插件引用
    IBasePlugin *m_core;

    // 核心服务管理器
    mutable CoreEventBus *m_eventManager;        ///< 事件总线管理器
    mutable DatabaseManager *m_databaseManager;  ///< 数据库管理器
    mutable LogManager *m_logManager;            ///< 日志管理器
    mutable ConfigManager *m_configManager;       ///< 配置管理器
    mutable AuthManager *m_authManager;           ///< 认证管理器
    mutable HttpClientManager *m_httpClientManager; ///< HTTP客户端管理器
    mutable ThemeManager *m_themeManager;           ///< 主题管理器
};

#endif // BASECOREPLUGIN_H

/**
 * @file IBasePlugin.h
 * @brief 插件基础接口定义
 * @details 定义所有插件必须实现的接口，包括生命周期管理、依赖注入和核心服务获取
 */

#ifndef IBASEPLUGIN_H
#define IBASEPLUGIN_H

#include "IBaseEventBus.h"
#include "IDatabaseManager.h"
#include "IConfigManager.h"
#include "ILogManager.h"
#include "IAuthManager.h"
#include "IHttpClientManager.h"

/**
 * @class IBasePlugin
 * @brief 插件基础接口类
 * @details 所有插件必须继承此接口并实现其纯虚函数
 * 
 * 插件生命周期：
 * 1. 插件管理器加载插件
 * 2. 调用 initialize() 进行初始化
 * 3. 调用 startPlugin() 启动插件
 * 4. 插件运行中...
 * 5. 调用 stopPlugin() 停止插件
 * 6. 调用 cleanup() 清理资源
 */
class IBasePlugin
{
public:
    /**
     * @brief 析构函数
     */
    virtual ~IBasePlugin() = default;

    // ===== 插件基本信息 =====

    /**
     * @brief 获取插件名称
     * @return QString 插件名称
     */
    virtual QString name() const = 0;

    /**
     * @brief 获取插件版本号
     * @return QString 版本号，格式如 "1.0.0"
     */
    virtual QString version() const = 0;

    /**
     * @brief 获取插件描述
     * @return QString 插件功能描述
     */
    virtual QString description() const = 0;

    /**
     * @brief 获取插件作者
     * @return QString 作者名称
     */
    virtual QString author() const = 0;

    // ===== 核心插件设置 =====

    /**
     * @brief 设置核心插件引用
     * @param core 核心插件指针
     * @note 插件管理器会在插件初始化前调用此方法
     */
    virtual void setCore(IBasePlugin *core) = 0;

    /**
     * @brief 获取核心插件引用
     * @return IBasePlugin* 核心插件指针
     */
    virtual IBasePlugin *core() const = 0;

    // ===== 依赖管理 =====

    /**
     * @brief 获取插件依赖列表
     * @return QStringList 依赖的其他插件名称列表
     */
    virtual QStringList dependencies() const = 0;

    // ===== 核心服务获取 =====

    /**
     * @brief 获取事件总线管理器
     * @return IBaseEventBus* 事件总线管理器指针
     */
    virtual IBaseEventBus *eventManager() const = 0;

    /**
     * @brief 获取数据库管理器
     * @return IDatabaseManager* 数据库管理器指针
     */
    virtual IDatabaseManager *databaseManager() const = 0;

    /**
     * @brief 获取日志管理器
     * @return ILogManager* 日志管理器指针
     */
    virtual ILogManager *logManager() const = 0;

    /**
     * @brief 获取认证管理器
     * @return IAuthManager* 认证管理器指针
     */
    virtual IAuthManager *authManager() const = 0;

    /**
     * @brief 获取HTTP客户端管理器
     * @return IHttpClientManager* HTTP客户端管理器指针
     */
    virtual IHttpClientManager *httpClientManager() const = 0;

    /**
     * @brief 获取配置管理器
     * @return IConfigManager* 配置管理器指针
     */
    virtual IConfigManager *configManager() const = 0;

    // ===== 插件生命周期管理 =====

    /**
     * @brief 初始化插件
     * @details 在插件加载时调用，用于一次性初始化操作
     * @return bool 初始化是否成功
     * @note 此方法在 startPlugin() 之前调用
     */
    virtual bool initialize() = 0;

    /**
     * @brief 启动插件
     * @details 在插件正式运行时调用，用于启动插件功能
     * @return bool 启动是否成功
     */
    virtual bool startPlugin() = 0;

    /**
     * @brief 停止插件
     * @details 在插件停止时调用，用于暂停插件功能
     * @return bool 停止是否成功
     */
    virtual bool stopPlugin() = 0;

    /**
     * @brief 清理插件资源
     * @details 在插件卸载时调用，用于释放资源
     */
    virtual void cleanup() = 0;
};

// Qt插件元数据接口ID
#define IBASEPLUGIN_IID "com.jxs.teacherMgrSys.IBasePlugin"

/**
 * @brief Qt插件接口声明宏
 * @note 使用此宏在插件类的 Q_OBJECT 之后声明实现此接口
 */
Q_DECLARE_INTERFACE(IBasePlugin, IBASEPLUGIN_IID)

#endif // IBASEPLUGIN_H

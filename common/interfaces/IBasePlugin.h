#ifndef IBASEPLUGIN_H
#define IBASEPLUGIN_H

#include "interfaces/IBaseEventBus.h"
#include "interfaces/IDatabaseManager.h"
#include "interfaces/IConfigManager.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QWidget>

/**
 * @brief 插件接口基类
 * 所有Qt插件必须实现此接口
 */
class IBasePlugin
{
public:
    virtual ~IBasePlugin() = default;

    virtual QString name() const = 0;           // 插件名称
    virtual QString version() const = 0;        // 插件版本
    virtual QString description() const = 0;     // 插件描述
    virtual QString author() const = 0;          // 插件作者

    virtual bool initialize() = 0;               // 初始化插件
    virtual bool startPlugin() = 0;             // 启动插件
    virtual bool stopPlugin() = 0;                    // 停止插件
    virtual void cleanup() = 0;                 // 清理资源

    virtual QVariant getConfig(const QString &key, const QVariant &defaultValue = QVariant()) const = 0;
    virtual void setConfig(const QString &key, const QVariant &value) = 0;
    virtual bool loadConfig() = 0;              // 加载配置
    virtual bool saveConfig() = 0;              // 保存配置

    // 事件管理
    virtual IBaseEventBus *eventManager() const = 0;

    // 数据库管理
    virtual IDatabaseManager *databaseManager() const = 0;

    // 配置管理
    virtual IConfigManager *configManager() const = 0;

    // 依赖管理
    virtual QStringList dependencies() const = 0; // 插件依赖
    
    // 界面管理
    virtual QWidget *mainWidget() const = 0; // 插件主窗口
};

// 插件接口标识
#define IBASEPLUGIN_IID "com.jxs.teacherMgrSys.IBasePlugin"

Q_DECLARE_INTERFACE(IBasePlugin, IBASEPLUGIN_IID)

#endif // IBASEPLUGIN_H

#ifndef BASECOREPLUGIN_H
#define BASECOREPLUGIN_H

#include "BaseCorePlugin_global.h"
#include "../../common/interfaces/IBasePlugin.h"
#include "EventManager.h"
#include "DatabaseManager.h"
#include "ConfigManager.h"

/**
 * @brief 基础核心插件
 * 实现了IBasePlugin接口，提供基本的插件功能
 */
class BASECOREPLUGIN_EXPORT BaseCorePlugin : public QObject, public IBasePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID IBASEPLUGIN_IID)
    Q_INTERFACES(IBasePlugin)

public:
    explicit BaseCorePlugin(QObject *parent = nullptr);
    ~BaseCorePlugin() override;

    QString name() const override;
    QString version() const override;
    QString description() const override;
    QString author() const override;


    bool initialize() override;
    bool startPlugin() override;
    bool stopPlugin() override;
    void cleanup() override;

    // 配置管理
    QVariant getConfig(const QString &key, const QVariant &defaultValue = QVariant()) const override;
    void setConfig(const QString &key, const QVariant &value) override;
    bool loadConfig() override;
    bool saveConfig() override;

    // 事件管理
    IBaseEventBus *eventManager() const override;

    // 数据库管理
    IDatabaseManager *databaseManager() const;

    // 配置管理
    IConfigManager *configManager() const;

    // 依赖管理
    QStringList dependencies() const override;

private:
    QMap<QString, QVariant> m_config;
    IBaseEventBus *m_eventManager;
    mutable DatabaseManager *m_databaseManager;
    mutable ConfigManager *m_configManager;
};

#endif // BASECOREPLUGIN_H

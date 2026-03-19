#ifndef CONFIGUIPLUGIN_H
#define CONFIGUIPLUGIN_H

#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IPluginWidget.h"
#include "../../common/interfaces/IAuthManager.h"
#include "../../common/interfaces/IHttpClientManager.h"
#include "../../common/interfaces/IThemeManager.h"

class ConfigWidget;

/**
 * @brief 配置管理界面插件
 * 提供配置管理的用户界面
 */
class ConfigUIPlugin : public QObject, public IBasePlugin, public IPluginWidget
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.ConfigUIPlugin" FILE "configuiplugin.json")
    Q_INTERFACES(IBasePlugin IPluginWidget)

public:
    ConfigUIPlugin(QObject *parent = nullptr);
    ~ConfigUIPlugin() override;

    // IBasePlugin 接口实现
    QString name() const override;
    QString version() const override;
    QString description() const override;
    bool initialize() override;
    bool startPlugin() override;
    bool stopPlugin() override;
    void cleanup() override;
    IBaseEventBus *eventManager() const override;
    IDatabaseManager *databaseManager() const override;
    ILogManager *logManager() const override;
    IAuthManager *authManager() const override;
    IHttpClientManager *httpClientManager() const override;
    IConfigManager *configManager() const override;
    IThemeManager *themeManager() const override;
    QStringList dependencies() const override;
    QString author() const override;

    // IPluginWidget interface
    QWidget *widget() const override;
    QString widgetTitle() const override;
    QIcon widgetIcon() const override;

    void setConfigManager(IConfigManager *configManager);

    void setEventManager(IBaseEventBus *eventManager);

    void setCore(IBasePlugin *core) override;
    IBasePlugin *core() const override;

private:
    IBasePlugin *m_core;
    ConfigWidget *m_configWidget;
    IBaseEventBus *m_eventManager;
    IDatabaseManager *m_databaseManager;
    ILogManager *m_logManager;
    IAuthManager *m_authManager;
    IConfigManager *m_configManager;
    IThemeManager *m_themeManager;
};

#endif // CONFIGUIPLUGIN_H

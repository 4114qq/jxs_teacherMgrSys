#ifndef HTTPDATAUIPLUGIN_H
#define HTTPDATAUIPLUGIN_H

#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IPluginWidget.h"
#include "../../common/interfaces/IHttpClientManager.h"

class HttpDataWidget;

class HttpDataUIPlugin : public QObject, public IBasePlugin, public IPluginWidget
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.HttpDataUIPlugin" FILE "httpdatauiplugin.json")
    Q_INTERFACES(IBasePlugin IPluginWidget)

public:
    HttpDataUIPlugin(QObject *parent = nullptr);
    ~HttpDataUIPlugin() override;

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
    QStringList dependencies() const override;
    QString author() const override;

    QWidget *widget() const override;
    QString widgetTitle() const override;
    QIcon widgetIcon() const override;

    void setHttpClientManager(IHttpClientManager *manager);
    void setConfigManager(IConfigManager *configManager);
    void setEventManager(IBaseEventBus *eventManager);
    void setCore(IBasePlugin *core) override;
    IBasePlugin *core() const override;

private:
    IBasePlugin *m_core;
    HttpDataWidget *m_httpDataWidget;
    IBaseEventBus *m_eventManager;
    IDatabaseManager *m_databaseManager;
    ILogManager *m_logManager;
    IAuthManager *m_authManager;
    IHttpClientManager *m_httpClientManager;
    IConfigManager *m_configManager;
};

#endif // HTTPDATAUIPLUGIN_H

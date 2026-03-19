#ifndef THEMUIPLUGIN_H
#define THEMUIPLUGIN_H

#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IPluginWidget.h"
#include "../../common/interfaces/IThemeManager.h"

class ThemeWidget;

class ThemeUIPlugin : public QObject, public IBasePlugin, public IPluginWidget
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.ThemeUIPlugin" FILE "themeuiplugin.json")
    Q_INTERFACES(IBasePlugin IPluginWidget)

public:
    ThemeUIPlugin(QObject *parent = nullptr);
    ~ThemeUIPlugin() override;

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

    QWidget *widget() const override;
    QString widgetTitle() const override;
    QIcon widgetIcon() const override;

    void setCore(IBasePlugin *core) override;
    IBasePlugin *core() const override;

private:
    IBasePlugin *m_core;
    ThemeWidget *m_themeWidget;
    IThemeManager *m_themeManager;
};

#endif // THEMUIPLUGIN_H

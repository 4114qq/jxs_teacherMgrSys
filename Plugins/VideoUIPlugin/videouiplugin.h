#ifndef VIDEOUIPLUGIN_H
#define VIDEOUIPLUGIN_H

#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IPluginWidget.h"
#include "../../common/interfaces/IThemeManager.h"

class VideoWidget;

class VideoUIPlugin : public QObject, public IBasePlugin, public IPluginWidget
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.VideoUIPlugin" FILE "videouiplugin.json")
    Q_INTERFACES(IBasePlugin IPluginWidget)

public:
    VideoUIPlugin(QObject *parent = nullptr);
    ~VideoUIPlugin() override;

    QString name() const override;
    QString version() const override;
    QString description() const override;
    QString author() const override;
    IBasePlugin *core() const override;
    QStringList dependencies() const override;

    bool initialize() override;
    void setCore(IBasePlugin *core) override;
    bool startPlugin() override;
    bool stopPlugin() override;
    void cleanup() override;

    IBaseEventBus *eventManager() const override;
    IConfigManager *configManager() const override;
    IDatabaseManager *databaseManager() const override;
    ILogManager *logManager() const override;
    IAuthManager *authManager() const override;
    IHttpClientManager *httpClientManager() const override;
    IThemeManager *themeManager() const override;

    QWidget *widget() const override;
    QString widgetTitle() const override;
    QIcon widgetIcon() const override;

private:
    IBasePlugin *m_core;
    VideoWidget *m_widget;
    IThemeManager *m_themeManager;
};

#endif // VIDEOUIPLUGIN_H

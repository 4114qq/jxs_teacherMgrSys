#ifndef COURSEVISUALPLUGIN_H
#define COURSEVISUALPLUGIN_H

#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IPluginWidget.h"

class CourseVisualWidget;

class CourseVisualPlugin : public QObject, public IBasePlugin, public IPluginWidget
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.CourseVisualPlugin" FILE "coursevisualplugin.json")
    Q_INTERFACES(IBasePlugin IPluginWidget)

public:
    CourseVisualPlugin(QObject *parent = nullptr);
    ~CourseVisualPlugin() override;

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
    CourseVisualWidget *m_courseVisualWidget;

};

#endif // COURSEVISUALPLUGIN_H

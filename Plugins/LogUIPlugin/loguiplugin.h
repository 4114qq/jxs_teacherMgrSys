#ifndef LOGUIPLUGIN_H
#define LOGUIPLUGIN_H

#include <QObject>
#include <QVariantMap>
#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IPluginWidget.h"
#include "../../common/interfaces/ILogManager.h"
#include "../../common/interfaces/IAuthManager.h"

class LogWidget;

class LogUIPlugin : public QObject, public IBasePlugin, public IPluginWidget
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.LogUIPlugin" FILE "loguiplugin.json")
    Q_INTERFACES(IBasePlugin IPluginWidget)

public:
    LogUIPlugin(QObject *parent = nullptr);
    ~LogUIPlugin() override;

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
    IConfigManager *configManager() const override;
    QStringList dependencies() const override;
    QString author() const override;
    QVariant getConfig(const QString &key, const QVariant &defaultValue) const override;
    void setConfig(const QString &key, const QVariant &value) override;
    bool loadConfig() override;
    bool saveConfig() override;

    QWidget *widget() const override;
    QString widgetTitle() const override;
    QIcon widgetIcon() const override;

    void setLogManager(ILogManager *manager);

    void setCore(IBasePlugin *core) override;
    IBasePlugin *core() const override;

private:
    IBasePlugin *m_core;
    LogWidget *m_logWidget;
    ILogManager *m_logManager;
    IAuthManager *m_authManager;
    IDatabaseManager *m_databaseManager;
    IBaseEventBus *m_eventManager;
    QVariantMap m_config;
};

#endif // LOGUIPLUGIN_H

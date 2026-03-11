#ifndef EVENTUIPLUGIN_H
#define EVENTUIPLUGIN_H

#include <QObject>
#include <QVariantMap>
#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IPluginWidget.h"
#include "../../common/interfaces/IAuthManager.h"

class EventWidget;

class EventUIPlugin : public QObject, public IBasePlugin, public IPluginWidget
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.EventUIPlugin" FILE "eventuiplugin.json")
    Q_INTERFACES(IBasePlugin IPluginWidget)

public:
    EventUIPlugin(QObject *parent = nullptr);
    ~EventUIPlugin() override;

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

    void setEventManager(IBaseEventBus *manager);

    void setCore(IBasePlugin *core) override;
    IBasePlugin *core() const override;

private:
    IBasePlugin *m_core;
    EventWidget *m_eventWidget;
    IBaseEventBus *m_eventManager;
    IDatabaseManager *m_databaseManager;
    ILogManager *m_logManager;
    IAuthManager *m_authManager;
    IConfigManager *m_configManager;
    QVariantMap m_config;

};

#endif // EVENTUIPLUGIN_H

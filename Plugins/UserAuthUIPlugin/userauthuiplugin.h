#ifndef USERAUTHUIPLUGIN_H
#define USERAUTHUIPLUGIN_H

#include <QObject>
#include <QVariantMap>
#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IPluginWidget.h"
#include "../../common/interfaces/IAuthManager.h"
#include "../../common/interfaces/ILogManager.h"

class UserAuthWidget;

class UserAuthUIPlugin : public QObject, public IBasePlugin, public IPluginWidget
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.UserAuthUIPlugin" FILE "userauthuiplugin.json")
    Q_INTERFACES(IBasePlugin IPluginWidget)

public:
    UserAuthUIPlugin(QObject *parent = nullptr);
    ~UserAuthUIPlugin() override;

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
    IConfigManager *configManager() const override;
    IAuthManager *authManager() const override;
    QStringList dependencies() const override;
    QString author() const override;
    QVariant getConfig(const QString &key, const QVariant &defaultValue) const override;
    void setConfig(const QString &key, const QVariant &value) override;
    bool loadConfig() override;
    bool saveConfig() override;

    QWidget *widget() const override;
    QString widgetTitle() const override;
    QIcon widgetIcon() const override;

    void setAuthManager(IAuthManager *manager);
    void setLogManager(ILogManager *manager);

    void setCore(IBasePlugin *core) override;
    IBasePlugin *core() const override;

private:
    IBasePlugin *m_core;
    UserAuthWidget *m_userAuthWidget;
    IAuthManager *m_authManager;
    ILogManager *m_logManager;
    IDatabaseManager *m_databaseManager;
    IConfigManager *m_configManager;
    QVariantMap m_config;

};

#endif // USERAUTHUIPLUGIN_H

#ifndef BASECOREPLUGIN_H
#define BASECOREPLUGIN_H

#include "BaseCorePlugin_global.h"
#include "../../common/interfaces/IBasePlugin.h"
#include "DatabaseManager.h"
#include "ConfigManager.h"
#include "LogManager.h"
#include "AuthManager.h"
#include "HttpClientManager.h"
#include "../../common/interfaces/IHttpClientManager.h"

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

    IBaseEventBus *eventManager() const override;

    IDatabaseManager *databaseManager() const override;

    IConfigManager *configManager() const override;

    QStringList dependencies() const override;

    ILogManager *logManager() const override;
    IAuthManager *authManager() const override;
    IHttpClientManager *httpClientManager() const override;

    void setCore(IBasePlugin *core) override;
    IBasePlugin *core() const override;

private:
    IBasePlugin *m_core;
    IBaseEventBus *m_eventManager;
    mutable DatabaseManager *m_databaseManager;
    mutable LogManager *m_logManager;
    mutable ConfigManager *m_configManager;
    mutable AuthManager *m_authManager;
    mutable HttpClientManager *m_httpClientManager;

};

#endif // BASECOREPLUGIN_H

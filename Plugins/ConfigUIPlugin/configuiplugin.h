#ifndef CONFIGUIPLUGIN_H
#define CONFIGUIPLUGIN_H

#include "../../common/interfaces/IBasePlugin.h"

class ConfigWidget;

/**
 * @brief 配置管理界面插件
 * 提供配置管理的用户界面
 */
class ConfigUIPlugin : public QObject, public IBasePlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.example.ConfigUIPlugin" FILE "configuiplugin.json")
    Q_INTERFACES(IBasePlugin)

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
    IConfigManager *configManager() const override;
    QWidget *mainWidget() const override;
    QStringList dependencies() const override;
    QString author() const override;
    QVariant getConfig(const QString &key, const QVariant &defaultValue) const override;
    void setConfig(const QString &key, const QVariant &value) override;
    bool loadConfig() override;
    bool saveConfig() override;

private:
    ConfigWidget *m_configWidget;
    IBaseEventBus *m_eventManager;
    IDatabaseManager *m_databaseManager;
    IConfigManager *m_configManager;


};

#endif // CONFIGUIPLUGIN_H

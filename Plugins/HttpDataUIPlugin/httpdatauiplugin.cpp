#include "httpdatauiplugin.h"
#include "httpdatawidget.h"
#include <QMessageBox>

HttpDataUIPlugin::HttpDataUIPlugin(QObject *parent)
    : QObject(parent)
    , m_core(nullptr)
    , m_httpDataWidget(nullptr)
    , m_eventManager(nullptr)
    , m_databaseManager(nullptr)
    , m_logManager(nullptr)
    , m_authManager(nullptr)
    , m_httpClientManager(nullptr)
    , m_configManager(nullptr)
{
}

HttpDataUIPlugin::~HttpDataUIPlugin()
{
    if (m_httpDataWidget) {
        delete m_httpDataWidget;
        m_httpDataWidget = nullptr;
    }
}

QString HttpDataUIPlugin::name() const
{
    return "HttpDataUIPlugin";
}

QString HttpDataUIPlugin::version() const
{
    return "1.0.0";
}

QString HttpDataUIPlugin::description() const
{
    return QString::fromLocal8Bit("HTTP数据管理界面插件 - 管理课程和设备线路数据");
}

bool HttpDataUIPlugin::initialize()
{
    return true;
}

bool HttpDataUIPlugin::startPlugin()
{
    if (!m_core) {
        return false;
    }

    m_httpClientManager = m_core->httpClientManager();
    if (!m_httpClientManager) {
        return false;
    }

    m_httpDataWidget = new HttpDataWidget();
    m_httpDataWidget->setHttpClientManager(m_httpClientManager);
    if (m_configManager) {
        m_httpDataWidget->setConfigManager(m_configManager);
    }

    return true;
}

bool HttpDataUIPlugin::stopPlugin()
{
    return true;
}

void HttpDataUIPlugin::cleanup()
{
    if (m_httpDataWidget) {
        delete m_httpDataWidget;
        m_httpDataWidget = nullptr;
    }
}

IBaseEventBus *HttpDataUIPlugin::eventManager() const
{
    return m_eventManager;
}

IDatabaseManager *HttpDataUIPlugin::databaseManager() const
{
    return m_databaseManager;
}

ILogManager *HttpDataUIPlugin::logManager() const
{
    return m_logManager;
}

IAuthManager *HttpDataUIPlugin::authManager() const
{
    return m_authManager;
}

IHttpClientManager *HttpDataUIPlugin::httpClientManager() const
{
    return m_httpClientManager;
}

IConfigManager *HttpDataUIPlugin::configManager() const
{
    return m_configManager;
}

QStringList HttpDataUIPlugin::dependencies() const
{
    return QStringList() << "BaseCorePlugin";
}

QString HttpDataUIPlugin::author() const
{
    return "jxs";
}

QWidget *HttpDataUIPlugin::widget() const
{
    return m_httpDataWidget;
}

QString HttpDataUIPlugin::widgetTitle() const
{
    return QString::fromLocal8Bit("HTTP数据管理");
}

QIcon HttpDataUIPlugin::widgetIcon() const
{
    return QIcon();
}

void HttpDataUIPlugin::setHttpClientManager(IHttpClientManager *manager)
{
    m_httpClientManager = manager;
    if (m_httpDataWidget) {
        m_httpDataWidget->setHttpClientManager(manager);
    }
}

void HttpDataUIPlugin::setConfigManager(IConfigManager *configManager)
{
    m_configManager = configManager;
    if (m_httpDataWidget) {
        m_httpDataWidget->setConfigManager(configManager);
    }
}

void HttpDataUIPlugin::setEventManager(IBaseEventBus *eventManager)
{
    m_eventManager = eventManager;
}

void HttpDataUIPlugin::setCore(IBasePlugin *core)
{
    m_core = core;
    if (m_core) {
        m_configManager = m_core->configManager();
    }
}

IBasePlugin *HttpDataUIPlugin::core() const
{
    return m_core;
}

IThemeManager *HttpDataUIPlugin::themeManager() const
{
    if (m_core) {
        return m_core->themeManager();
    }
    return nullptr;
}

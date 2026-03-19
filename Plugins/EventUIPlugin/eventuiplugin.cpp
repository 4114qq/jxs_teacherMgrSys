#include "eventuiplugin.h"
#include "eventwidget.h"
#include "../../common/interfaces/IBaseEventBus.h"
#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/ILogManager.h"

EventUIPlugin::EventUIPlugin(QObject *parent)
    : QObject(parent)
    , m_core(nullptr)
    , m_eventWidget(nullptr)
    , m_eventManager(nullptr)
    , m_databaseManager(nullptr)
    , m_logManager(nullptr)
    , m_authManager(nullptr)
    , m_configManager(nullptr)
{
}

EventUIPlugin::~EventUIPlugin()
{
    if (m_eventWidget) {
        delete m_eventWidget;
        m_eventWidget = nullptr;
    }
}

QString EventUIPlugin::name() const
{
    return "EventUIPlugin";
}

QString EventUIPlugin::version() const
{
    return "1.0.0";
}

QString EventUIPlugin::description() const
{
    return "Event monitoring and communication UI plugin";
}

QString EventUIPlugin::author() const
{
    return "Developer";
}

bool EventUIPlugin::initialize()
{
    return true;
}

bool EventUIPlugin::startPlugin()
{
    if (m_core) {
        m_eventManager = m_core->eventManager();
    }

    if (!m_eventWidget) {
        m_eventWidget = new EventWidget();
    }
    if (m_eventManager) {
        m_eventWidget->setEventManager(m_eventManager);
    }
    return true;
}

bool EventUIPlugin::stopPlugin()
{
    return true;
}

void EventUIPlugin::cleanup()
{
}

IBaseEventBus *EventUIPlugin::eventManager() const
{
    return m_eventManager;
}

IDatabaseManager *EventUIPlugin::databaseManager() const
{
    return m_databaseManager;
}

ILogManager *EventUIPlugin::logManager() const
{
    return m_logManager;
}

IAuthManager *EventUIPlugin::authManager() const
{
    return m_authManager;
}

IHttpClientManager *EventUIPlugin::httpClientManager() const
{
    return m_core->httpClientManager();
}

IConfigManager *EventUIPlugin::configManager() const
{
    return nullptr;
}

QStringList EventUIPlugin::dependencies() const
{
    return QStringList() << "BaseCorePlugin";
}

QWidget *EventUIPlugin::widget() const
{
    return m_eventWidget;
}

QString EventUIPlugin::widgetTitle() const
{
    return QString::fromLocal8Bit("事件监控");
}

QIcon EventUIPlugin::widgetIcon() const
{
    return QIcon();
}

void EventUIPlugin::setCore(IBasePlugin *core)
{
    m_core = core;

    if (m_core) {
        m_eventManager = m_core->eventManager();
        if (m_eventWidget && m_eventManager) {
            m_eventWidget->setEventManager(m_eventManager);
        }
    }
}

IBasePlugin *EventUIPlugin::core() const
{
    return m_core;
}

IThemeManager *EventUIPlugin::themeManager() const
{
    if (m_core) {
        return m_core->themeManager();
    }
    return nullptr;
}

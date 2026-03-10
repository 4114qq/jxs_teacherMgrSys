#include "eventuiplugin.h"
#include "eventwidget.h"
#include "../../common/interfaces/IBaseEventBus.h"
#include "../../common/interfaces/ILogManager.h"

EventUIPlugin::EventUIPlugin(QObject *parent)
    : QObject(parent)
    , m_eventWidget(nullptr)
    , m_eventManager(nullptr)
    , m_databaseManager(nullptr)
    , m_logManager(nullptr)
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
    QVariant var = getConfig("eventManagerPtr", QVariant());
    if (!var.isNull()) {
        IBaseEventBus *eventMgr = static_cast<IBaseEventBus*>(var.value<void*>());
        m_eventManager = eventMgr;
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

IConfigManager *EventUIPlugin::configManager() const
{
    return nullptr;
}

QStringList EventUIPlugin::dependencies() const
{
    return QStringList() << "BaseCorePlugin";
}

QVariant EventUIPlugin::getConfig(const QString &key, const QVariant &defaultValue) const
{
    return m_config.value(key, defaultValue);
}

void EventUIPlugin::setConfig(const QString &key, const QVariant &value)
{
    m_config[key] = value;
}

bool EventUIPlugin::loadConfig()
{
    return true;
}

bool EventUIPlugin::saveConfig()
{
    return true;
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

void EventUIPlugin::setEventManager(IBaseEventBus *manager)
{
    m_eventManager = manager;
    if (m_eventWidget) {
        m_eventWidget->setEventManager(manager);
    }
}

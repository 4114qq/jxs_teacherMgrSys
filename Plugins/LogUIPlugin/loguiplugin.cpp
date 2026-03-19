#include "loguiplugin.h"
#include "logwidget.h"
#include <QIcon>

LogUIPlugin::LogUIPlugin(QObject *parent)
    : QObject(parent)
    , m_core(nullptr)
    , m_logWidget(nullptr)
    , m_logManager(nullptr)
    , m_authManager(nullptr)
    , m_databaseManager(nullptr)
    , m_eventManager(nullptr)
{
}

LogUIPlugin::~LogUIPlugin()
{
    if (m_logWidget) {
        delete m_logWidget;
        m_logWidget = nullptr;
    }
}

QString LogUIPlugin::name() const
{
    return "LogUIPlugin";
}

QString LogUIPlugin::version() const
{
    return "1.0.0";
}

QString LogUIPlugin::description() const
{
    return "Log Management UI Plugin";
}

bool LogUIPlugin::initialize()
{
    m_logWidget = new LogWidget();
    if (m_logManager) {
        m_logWidget->setLogManager(m_logManager);
    }
    return true;
}

bool LogUIPlugin::startPlugin()
{
    return true;
}

bool LogUIPlugin::stopPlugin()
{
    return true;
}

void LogUIPlugin::cleanup()
{
}

IBaseEventBus *LogUIPlugin::eventManager() const
{
    return m_eventManager;
}

IDatabaseManager *LogUIPlugin::databaseManager() const
{
    return m_databaseManager;
}

ILogManager *LogUIPlugin::logManager() const
{
    return m_logManager;
}

IAuthManager *LogUIPlugin::authManager() const
{
    return m_authManager;
}

IHttpClientManager *LogUIPlugin::httpClientManager() const
{
    return m_core->httpClientManager();
}

IConfigManager *LogUIPlugin::configManager() const
{
    return nullptr;
}

QStringList LogUIPlugin::dependencies() const
{
    return QStringList() << "BaseCorePlugin";
}

QString LogUIPlugin::author() const
{
    return "Developer";
}

QWidget *LogUIPlugin::widget() const
{
    return m_logWidget;
}

QString LogUIPlugin::widgetTitle() const
{
    return QString::fromLocal8Bit("日志管理");
}

QIcon LogUIPlugin::widgetIcon() const
{
    return QIcon();
}

void LogUIPlugin::setCore(IBasePlugin *core)
{
    m_core = core;

    if (m_core) {
        m_logManager = m_core->logManager();
        m_themeManager = m_core->themeManager();
        IBaseEventBus *eventBus = m_core->eventManager();

        if (m_logWidget && m_logManager) {
            m_logWidget->setLogManager(m_logManager);
        }

        if (m_themeManager && eventBus) {
            IBaseEventBus::Subscription sub;
            sub.receiver = this;
            sub.slot = "handleThemeChanged";
            eventBus->subscribe("theme.changed", sub);
            applyPluginStyle();
        }
    }
}

void LogUIPlugin::handleThemeChanged(const QVariantMap &data)
{
    QString themeName = data["theme"].toString();
    Q_UNUSED(themeName);
    applyPluginStyle();
}

void LogUIPlugin::applyPluginStyle()
{
    if (!m_themeManager || !m_logWidget) {
        return;
    }

    QString pluginStyle = m_themeManager->loadFile("themes/plugins/logui.qss");
    m_logWidget->setStyleSheet(pluginStyle);
}

IBasePlugin *LogUIPlugin::core() const
{
    return m_core;
}

IThemeManager *LogUIPlugin::themeManager() const
{
    if (m_core) {
        return m_core->themeManager();
    }
    return nullptr;
}

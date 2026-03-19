#include "databaseuiplugin.h"
#include "databasewidget.h"
#include "../../common/interfaces/ILogManager.h"
#include <QIcon>

DatabaseUIPlugin::DatabaseUIPlugin(QObject *parent)
    : QObject(parent)
    , m_core(nullptr)
    , m_databaseWidget(nullptr)
    , m_databaseManager(nullptr)
    , m_logManager(nullptr)
    , m_authManager(nullptr)
    , m_eventManager(nullptr)
{
}

DatabaseUIPlugin::~DatabaseUIPlugin()
{
    if (m_databaseWidget) {
        delete m_databaseWidget;
        m_databaseWidget = nullptr;
    }
}

QString DatabaseUIPlugin::name() const
{
    return "DatabaseUIPlugin";
}

QString DatabaseUIPlugin::version() const
{
    return "1.0.0";
}

QString DatabaseUIPlugin::description() const
{
    return "Database Management UI Plugin";
}

bool DatabaseUIPlugin::initialize()
{
    m_databaseWidget = new DatabaseWidget();
    if (m_databaseManager) {
        m_databaseWidget->setDatabaseManager(m_databaseManager);
    }
    return true;
}

bool DatabaseUIPlugin::startPlugin()
{
    return true;
}

bool DatabaseUIPlugin::stopPlugin()
{
    return true;
}

void DatabaseUIPlugin::cleanup()
{
}

IBaseEventBus *DatabaseUIPlugin::eventManager() const
{
    return m_eventManager;
}

IDatabaseManager *DatabaseUIPlugin::databaseManager() const
{
    return m_databaseManager;
}

ILogManager *DatabaseUIPlugin::logManager() const
{
    return m_logManager;
}

IAuthManager *DatabaseUIPlugin::authManager() const
{
    return m_authManager;
}

IHttpClientManager *DatabaseUIPlugin::httpClientManager() const
{
    return m_core->httpClientManager();
}

IConfigManager *DatabaseUIPlugin::configManager() const
{
    return nullptr;
}

QStringList DatabaseUIPlugin::dependencies() const
{
    return QStringList() << "BaseCorePlugin";
}

QString DatabaseUIPlugin::author() const
{
    return "Developer";
}

QWidget *DatabaseUIPlugin::widget() const
{
    return m_databaseWidget;
}

QString DatabaseUIPlugin::widgetTitle() const
{
    return QString::fromLocal8Bit("database manager");
}

QIcon DatabaseUIPlugin::widgetIcon() const
{
    return QIcon();
}

void DatabaseUIPlugin::setDatabaseManager(IDatabaseManager *manager)
{
    m_databaseManager = manager;
    if (m_databaseWidget) {
        m_databaseWidget->setDatabaseManager(manager);
    }
}

void DatabaseUIPlugin::setEventManager(IBaseEventBus *manager)
{
    m_eventManager = manager;
}

void DatabaseUIPlugin::setCore(IBasePlugin *core)
{
    m_core = core;

    if (m_core) {
        m_databaseManager = m_core->databaseManager();
        if (m_databaseWidget && m_databaseManager) {
            m_databaseWidget->setDatabaseManager(m_databaseManager);
        }
    }
}

IBasePlugin *DatabaseUIPlugin::core() const
{
    return m_core;
}

IThemeManager *DatabaseUIPlugin::themeManager() const
{
    if (m_core) {
        return m_core->themeManager();
    }
    return nullptr;
}

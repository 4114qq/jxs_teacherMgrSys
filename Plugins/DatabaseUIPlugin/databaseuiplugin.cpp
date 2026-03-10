#include "databaseuiplugin.h"
#include "databasewidget.h"
#include "../../common/interfaces/ILogManager.h"
#include <QIcon>

DatabaseUIPlugin::DatabaseUIPlugin(QObject *parent)
    : QObject(parent)
    , m_databaseWidget(nullptr)
    , m_databaseManager(nullptr)
    , m_logManager(nullptr)
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

QVariant DatabaseUIPlugin::getConfig(const QString &key, const QVariant &defaultValue) const
{
    return m_config.value(key, defaultValue);
}

void DatabaseUIPlugin::setConfig(const QString &key, const QVariant &value)
{
    m_config[key] = value;

    if (key == "databaseManagerPtr") {
        m_databaseManager = static_cast<IDatabaseManager*>(value.value<void*>());
        if (m_databaseWidget && m_databaseManager) {
            m_databaseWidget->setDatabaseManager(m_databaseManager);
        }
    } else if (key == "eventManagerPtr") {
        m_eventManager = static_cast<IBaseEventBus*>(value.value<void*>());
    }
}

bool DatabaseUIPlugin::loadConfig()
{
    return true;
}

bool DatabaseUIPlugin::saveConfig()
{
    return true;
}

QWidget *DatabaseUIPlugin::widget() const
{
    return m_databaseWidget;
}

QString DatabaseUIPlugin::widgetTitle() const
{
    return QString::fromLocal8Bit("数据库管理");
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

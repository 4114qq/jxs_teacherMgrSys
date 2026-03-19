#include "userauthuiplugin.h"
#include "userauthwidget.h"
#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IAuthManager.h"
#include "../../common/interfaces/ILogManager.h"

UserAuthUIPlugin::UserAuthUIPlugin(QObject *parent)
    : QObject(parent)
    , m_core(nullptr)
    , m_userAuthWidget(nullptr)
    , m_authManager(nullptr)
    , m_logManager(nullptr)
    , m_databaseManager(nullptr)
    , m_configManager(nullptr)
{
}

UserAuthUIPlugin::~UserAuthUIPlugin()
{
    if (m_userAuthWidget) {
        delete m_userAuthWidget;
        m_userAuthWidget = nullptr;
    }
}

QString UserAuthUIPlugin::name() const
{
    return "UserAuthUIPlugin";
}

QString UserAuthUIPlugin::version() const
{
    return "1.0.0";
}

QString UserAuthUIPlugin::description() const
{
    return "User authentication and role management UI plugin";
}

QString UserAuthUIPlugin::author() const
{
    return "Developer";
}

bool UserAuthUIPlugin::initialize()
{
    m_userAuthWidget = new UserAuthWidget();
    return true;
}

bool UserAuthUIPlugin::startPlugin()
{
    if (m_authManager) {
        m_userAuthWidget->setAuthManager(m_authManager);
    }
    if (m_logManager) {
        m_userAuthWidget->setLogManager(m_logManager);
    }
    return true;
}

bool UserAuthUIPlugin::stopPlugin()
{
    return true;
}

void UserAuthUIPlugin::cleanup()
{
}

IBaseEventBus *UserAuthUIPlugin::eventManager() const
{
    return nullptr;
}

IDatabaseManager *UserAuthUIPlugin::databaseManager() const
{
    return m_databaseManager;
}

ILogManager *UserAuthUIPlugin::logManager() const
{
    return m_logManager;
}

IConfigManager *UserAuthUIPlugin::configManager() const
{
    return nullptr;
}

IAuthManager *UserAuthUIPlugin::authManager() const
{
    return m_authManager;
}

IHttpClientManager *UserAuthUIPlugin::httpClientManager() const
{
    return m_core->httpClientManager();
}

QStringList UserAuthUIPlugin::dependencies() const
{
    return QStringList() << "BaseCorePlugin";
}

QWidget *UserAuthUIPlugin::widget() const
{
    return m_userAuthWidget;
}

QString UserAuthUIPlugin::widgetTitle() const
{
    return QString::fromLocal8Bit("人员验证");
}

QIcon UserAuthUIPlugin::widgetIcon() const
{
    return QIcon();
}

void UserAuthUIPlugin::setAuthManager(IAuthManager *manager)
{
    m_authManager = manager;
    if (m_userAuthWidget) {
        m_userAuthWidget->setAuthManager(manager);
    }
}

void UserAuthUIPlugin::setLogManager(ILogManager *manager)
{
    m_logManager = manager;
    if (m_userAuthWidget) {
        m_userAuthWidget->setLogManager(manager);
    }
}

void UserAuthUIPlugin::setCore(IBasePlugin *core)
{
    m_core = core;

    if (m_core) {
        m_authManager = m_core->authManager();
        m_logManager = m_core->logManager();
        if (m_userAuthWidget) {
            if (m_authManager) {
                m_userAuthWidget->setAuthManager(m_authManager);
            }
            if (m_logManager) {
                m_userAuthWidget->setLogManager(m_logManager);
            }
        }
    }
}

IBasePlugin *UserAuthUIPlugin::core() const
{
    return m_core;
}

IThemeManager *UserAuthUIPlugin::themeManager() const
{
    if (m_core) {
        return m_core->themeManager();
    }
    return nullptr;
}



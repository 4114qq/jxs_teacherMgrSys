#include "themeuiplugin.h"
#include "themewidget.h"
#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IThemeManager.h"

ThemeUIPlugin::ThemeUIPlugin(QObject *parent)
    : QObject(parent)
    , m_core(nullptr)
    , m_themeWidget(nullptr)
    , m_themeManager(nullptr)
{
}

ThemeUIPlugin::~ThemeUIPlugin()
{
    cleanup();
}

QString ThemeUIPlugin::name() const
{
    return "ThemeUIPlugin";
}

QString ThemeUIPlugin::version() const
{
    return "1.0.0";
}

QString ThemeUIPlugin::description() const
{
    return "主题管理界面插件";
}

QString ThemeUIPlugin::author() const
{
    return "Your Name";
}

bool ThemeUIPlugin::initialize()
{
    m_themeWidget = new ThemeWidget();
    m_themeWidget->setWindowFlags(Qt::Widget);
    return true;
}

bool ThemeUIPlugin::startPlugin()
{
    return true;
}

bool ThemeUIPlugin::stopPlugin()
{
    return true;
}

void ThemeUIPlugin::cleanup()
{
    if (m_themeWidget) {
        delete m_themeWidget;
        m_themeWidget = nullptr;
    }
}

IBaseEventBus *ThemeUIPlugin::eventManager() const
{
    return nullptr;
}

IDatabaseManager *ThemeUIPlugin::databaseManager() const
{
    return nullptr;
}

ILogManager *ThemeUIPlugin::logManager() const
{
    return nullptr;
}

IAuthManager *ThemeUIPlugin::authManager() const
{
    return nullptr;
}

IHttpClientManager *ThemeUIPlugin::httpClientManager() const
{
    return nullptr;
}

IConfigManager *ThemeUIPlugin::configManager() const
{
    return nullptr;
}

QWidget *ThemeUIPlugin::widget() const
{
    return m_themeWidget;
}

QString ThemeUIPlugin::widgetTitle() const
{
    return "主题管理";
}

QIcon ThemeUIPlugin::widgetIcon() const
{
    return QIcon();
}

QStringList ThemeUIPlugin::dependencies() const
{
    return {"BaseCorePlugin"};
}

void ThemeUIPlugin::setCore(IBasePlugin *core)
{
    m_core = core;

    if (m_core) {
        m_themeManager = m_core->themeManager();
        IBaseEventBus *eventBus = m_core->eventManager();
        if (m_themeWidget && m_themeManager) {
            m_themeWidget->setThemeManager(m_themeManager);
            m_themeWidget->setEventBus(eventBus);
        }
    }
}

IThemeManager *ThemeUIPlugin::themeManager() const
{
    return m_themeManager;
}

IBasePlugin *ThemeUIPlugin::core() const
{
    return m_core;
}

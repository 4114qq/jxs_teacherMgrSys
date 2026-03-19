#include "coursevisualplugin.h"
#include "coursevisualwidget.h"
#include "../../common/interfaces/IBasePlugin.h"

CourseVisualPlugin::CourseVisualPlugin(QObject *parent)
    : QObject(parent)
    , m_core(nullptr)
    , m_courseVisualWidget(nullptr)
{
}

CourseVisualPlugin::~CourseVisualPlugin()
{
    cleanup();
}

QString CourseVisualPlugin::name() const
{
    return "CourseVisualPlugin";
}

QString CourseVisualPlugin::version() const
{
    return "1.0.0";
}

QString CourseVisualPlugin::description() const
{
    return QString::fromLocal8Bit("课程可视化编辑插件");
}

QString CourseVisualPlugin::author() const
{
    return "Your Name";
}

bool CourseVisualPlugin::initialize()
{
    m_courseVisualWidget = new CourseVisualWidget();
    m_courseVisualWidget->setWindowFlags(Qt::Widget);
    return true;
}

bool CourseVisualPlugin::startPlugin()
{
    return true;
}

bool CourseVisualPlugin::stopPlugin()
{
    return true;
}

void CourseVisualPlugin::cleanup()
{
    if (m_courseVisualWidget) {
        delete m_courseVisualWidget;
        m_courseVisualWidget = nullptr;
    }
}

IBaseEventBus *CourseVisualPlugin::eventManager() const
{
    return m_core ? m_core->eventManager() : nullptr;
}

IDatabaseManager *CourseVisualPlugin::databaseManager() const
{
    return m_core ? m_core->databaseManager() : nullptr;
}

ILogManager *CourseVisualPlugin::logManager() const
{
    return m_core ? m_core->logManager() : nullptr;
}

IAuthManager *CourseVisualPlugin::authManager() const
{
    return m_core ? m_core->authManager() : nullptr;
}

IHttpClientManager *CourseVisualPlugin::httpClientManager() const
{
    return m_core ? m_core->httpClientManager() : nullptr;
}

IConfigManager *CourseVisualPlugin::configManager() const
{
    return m_core ? m_core->configManager() : nullptr;
}

IThemeManager *CourseVisualPlugin::themeManager() const
{
    return m_core ? m_core->themeManager() : nullptr;
}

QWidget *CourseVisualPlugin::widget() const
{
    return m_courseVisualWidget;
}

QString CourseVisualPlugin::widgetTitle() const
{
    return QString::fromLocal8Bit("课程可视化编辑");
}

QIcon CourseVisualPlugin::widgetIcon() const
{
    return QIcon();
}

QStringList CourseVisualPlugin::dependencies() const
{
    return {"BaseCorePlugin"};
}

void CourseVisualPlugin::setCore(IBasePlugin *core)
{
    m_core = core;
}

IBasePlugin *CourseVisualPlugin::core() const
{
    return m_core;
}

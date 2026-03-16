#include "videouiplugin.h"
#include "videowidget.h"

#include <QIcon>

VideoUIPlugin::VideoUIPlugin(QObject *parent)
    : QObject(parent)
    , m_core(nullptr)
    , m_widget(nullptr)
{
}

VideoUIPlugin::~VideoUIPlugin()
{
    if (m_widget) {
        delete m_widget;
        m_widget = nullptr;
    }
}

QString VideoUIPlugin::name() const
{
    return "VideoUIPlugin";
}

QString VideoUIPlugin::version() const
{
    return "1.0.0";
}

QString VideoUIPlugin::description() const
{
    return "Video Player Plugin";
}

QString VideoUIPlugin::author() const
{
    return "Default";
}

IBasePlugin *VideoUIPlugin::core() const
{
    return m_core;
}

QStringList VideoUIPlugin::dependencies() const
{
    return QStringList() << "BaseCorePlugin";
}

bool VideoUIPlugin::initialize()
{
    return true;
}

void VideoUIPlugin::setCore(IBasePlugin *core)
{
    m_core = core;
}

bool VideoUIPlugin::startPlugin()
{
    if (!m_widget) {
        m_widget = new VideoWidget();
    }
    return true;
}

bool VideoUIPlugin::stopPlugin()
{
    if (m_widget) {
        delete m_widget;
        m_widget = nullptr;
    }
    return true;
}

void VideoUIPlugin::cleanup()
{
}

IBaseEventBus *VideoUIPlugin::eventManager() const
{
    if (m_core) {
        return m_core->eventManager();
    }
    return nullptr;
}

IConfigManager *VideoUIPlugin::configManager() const
{
    if (m_core) {
        return m_core->configManager();
    }
    return nullptr;
}

IDatabaseManager *VideoUIPlugin::databaseManager() const
{
    if (m_core) {
        return m_core->databaseManager();
    }
    return nullptr;
}

ILogManager *VideoUIPlugin::logManager() const
{
    if (m_core) {
        return m_core->logManager();
    }
    return nullptr;
}

IAuthManager *VideoUIPlugin::authManager() const
{
    if (m_core) {
        return m_core->authManager();
    }
    return nullptr;
}

IHttpClientManager *VideoUIPlugin::httpClientManager() const
{
    if (m_core) {
        return m_core->httpClientManager();
    }
    return nullptr;
}

QWidget *VideoUIPlugin::widget() const
{
    return m_widget;
}

QString VideoUIPlugin::widgetTitle() const
{
    return QString::fromLocal8Bit("视频播放");
}

QIcon VideoUIPlugin::widgetIcon() const
{
    return QIcon();
}

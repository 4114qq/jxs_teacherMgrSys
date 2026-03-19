/**
 * @file ThemeManager.cpp
 * @brief 主题管理器实现
 * @details 提供主题切换、样式加载等功能
 */

#include "ThemeManager.h"
#include "ConfigManager.h"
#include "../../common/interfaces/IBaseEventBus.h"
#include <QFile>
#include <QCoreApplication>
#include <QApplication>
#include <QDebug>
#include <QDir>

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentTheme("light")
    , m_configManager(nullptr)
    , m_eventBus(nullptr)
{
    m_resourcePath = qApp->applicationDirPath() + "/resources/";
}

ThemeManager::~ThemeManager()
{
}

void ThemeManager::initialize(IConfigManager *configManager, IBaseEventBus *eventBus)
{
    m_configManager = configManager;
    m_eventBus = eventBus;

    if (m_configManager) {
        QString savedTheme = m_configManager->get("Theme", "light").toString();
        setTheme(savedTheme);
    }
}

QString ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

void ThemeManager::setTheme(const QString &themeName)
{
    if (m_currentTheme == themeName) {
        return;
    }

    m_currentTheme = themeName;

    QString style = loadStylesheet(themeName);
    QApplication *app = qobject_cast<QApplication*>(QCoreApplication::instance());
    if (app) {
        app->setStyleSheet(style);
    }

    if (m_configManager) {
        m_configManager->set("Theme", themeName);
        m_configManager->save();
    }

    qDebug() << "Theme changed to:" << themeName;
}

QStringList ThemeManager::availableThemes() const
{
    QStringList themes;
    themes << "light" << "dark" << "blue";

    QString themeDir = m_resourcePath + "themes/base/";
    QDir dir(themeDir);
    if (dir.exists()) {
        QStringList filters;
        filters << "*.qss";
        QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
        for (const QFileInfo &file : files) {
            QString baseName = file.baseName();
            if (!themes.contains(baseName)) {
                themes.append(baseName);
            }
        }
    }

    return themes;
}

QString ThemeManager::loadFile(const QString &relativePath) const
{
    QString fullPath = m_resourcePath + relativePath;
    QFile file(fullPath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();
        return QString::fromUtf8(data);
    }

    qWarning() << "Cannot load file:" << fullPath;
    return QString();
}

QString ThemeManager::loadStylesheet(const QString &themeName) const
{
    QString filePath = getThemeFilePath(themeName);
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();
        QString style = QString::fromUtf8(data);
        qDebug() << "Loaded theme:" << themeName << "from" << filePath;
        return style;
    }

    qWarning() << "Cannot load theme file:" << filePath;
    return QString();
}

QString ThemeManager::getThemeFilePath(const QString &themeName) const
{
    return m_resourcePath + "themes/base/" + themeName + ".qss";
}

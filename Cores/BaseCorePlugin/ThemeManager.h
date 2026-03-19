/**
 * @file ThemeManager.h
 * @brief 主题管理器实现
 * @details 提供主题切换、样式加载等功能
 */

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include "BaseCorePlugin_global.h"
#include "../../common/interfaces/IThemeManager.h"
#include <QObject>
#include <QString>
#include <QStringList>

class IConfigManager;
class IBaseEventBus;

/**
 * @class ThemeManager
 * @brief 主题管理器
 * @details 提供主题切换、样式加载等功能
 */
class BASECOREPLUGIN_EXPORT ThemeManager : public QObject, public IThemeManager
{
    Q_OBJECT
    Q_INTERFACES(IThemeManager)

public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit ThemeManager(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~ThemeManager();

    /**
     * @brief 初始化
     * @param configManager 配置管理器指针
     * @param eventBus 事件总线指针
     */
    void initialize(IConfigManager *configManager, IBaseEventBus *eventBus);

    /**
     * @brief 获取当前主题
     * @return QString 当前主题名称
     */
    QString currentTheme() const override;

    /**
     * @brief 设置主题
     * @param themeName 主题名称
     */
    void setTheme(const QString &themeName) override;

    /**
     * @brief 获取可用主题列表
     * @return QStringList 可用主题名称列表
     */
    QStringList availableThemes() const override;

    /**
     * @brief 加载指定文件
     * @param relativePath 相对于资源目录的路径
     * @return QString 文件内容
     */
    QString loadFile(const QString &relativePath) const override;

private:
    /**
     * @brief 加载样式表
     * @param themeName 主题名称
     * @return QString 样式表内容
     */
    QString loadStylesheet(const QString &themeName) const;

    /**
     * @brief 获取主题文件路径
     * @param themeName 主题名称
     * @return QString 主题文件完整路径
     */
    QString getThemeFilePath(const QString &themeName) const;

private:
    QString m_currentTheme;              ///< 当前主题
    QString m_resourcePath;              ///< 资源路径
    IConfigManager *m_configManager;    ///< 配置管理器
    IBaseEventBus *m_eventBus;          ///< 事件总线
};

#endif // THEMEMANAGER_H

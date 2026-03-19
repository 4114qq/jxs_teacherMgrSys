/**
 * @file IThemeManager.h
 * @brief 主题管理器接口定义
 */

#ifndef ITHEMEMANAGER_H
#define ITHEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

/**
 * @class IThemeManager
 * @brief 主题管理器接口
 * @details 提供主题切换、样式加载等功能
 */
class IThemeManager
{
public:
    /**
     * @brief 析构函数
     */
    virtual ~IThemeManager() = default;

    /**
     * @brief 获取当前主题
     * @return QString 当前主题名称
     */
    virtual QString currentTheme() const = 0;

    /**
     * @brief 设置主题
     * @param themeName 主题名称
     */
    virtual void setTheme(const QString &themeName) = 0;

    /**
     * @brief 获取可用主题列表
     * @return QStringList 可用主题名称列表
     */
    virtual QStringList availableThemes() const = 0;

    /**
     * @brief 加载指定文件
     * @param relativePath 相对于资源目录的路径
     * @return QString 文件内容，读取失败返回空字符串
     */
    virtual QString loadFile(const QString &relativePath) const = 0;
};

#define ITHEMEMANAGER_IID "com.jxs.teacherMgrSys.IThemeManager"

Q_DECLARE_INTERFACE(IThemeManager, ITHEMEMANAGER_IID)

#endif // ITHEMEMANAGER_H

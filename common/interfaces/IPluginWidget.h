/**
 * @file IPluginWidget.h
 * @brief 插件界面接口定义
 * @details 定义插件窗口的抽象接口
 */

#ifndef IPLUGINWIDGET_H
#define IPLUGINWIDGET_H

#include <QWidget>
#include <QString>
#include <QIcon>

/**
 * @class IPluginWidget
 * @brief 插件界面接口
 * @details 提供插件窗口的抽象接口，用于在主界面中显示插件内容
 */
class IPluginWidget
{
public:
    /**
     * @brief 析构函数
     */
    virtual ~IPluginWidget() = default;

    // ===== 窗口信息 =====

    /**
     * @brief 获取插件窗口指针
     * @return QWidget* 窗口指针
     */
    virtual QWidget *widget() const = 0;

    /**
     * @brief 获取插件窗口标题
     * @return QString 窗口标题
     */
    virtual QString widgetTitle() const = 0;

    /**
     * @brief 获取插件窗口图标
     * @return QIcon 窗口图标
     */
    virtual QIcon widgetIcon() const = 0;

    // ===== 停靠窗口配置 =====

    /**
     * @brief 获取默认停靠区域
     * @return Qt::DockWidgetArea 默认停靠区域
     */
    virtual Qt::DockWidgetArea defaultDockArea() const { return Qt::RightDockWidgetArea; }

    /**
     * @brief 检查是否可关闭
     * @return bool 是否可关闭
     */
    virtual bool isClosable() const { return true; }

    /**
     * @brief 检查是否可浮动
     * @return bool 是否可浮动
     */
    virtual bool isFloatable() const { return true; }
};

// Qt插件元数据接口ID
#define IPLUGINWIDGET_IID "com.jxs.teacherMgrSys.IPluginWidget"

/**
 * @brief Qt插件接口声明宏
 */
Q_DECLARE_INTERFACE(IPluginWidget, IPLUGINWIDGET_IID)

#endif // IPLUGINWIDGET_H

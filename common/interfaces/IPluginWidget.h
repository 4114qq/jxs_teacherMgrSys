#ifndef IPLUGINWIDGET_H
#define IPLUGINWIDGET_H

#include <QWidget>
#include <QString>
#include <QIcon>

/**
 * @brief 插件界面接口
 * 提供插件窗口的抽象接口
 */
class IPluginWidget
{
public:
    virtual ~IPluginWidget() = default;

    virtual QWidget *widget() const = 0;
    virtual QString widgetTitle() const = 0;
    virtual QIcon widgetIcon() const = 0;
    virtual Qt::DockWidgetArea defaultDockArea() const { return Qt::RightDockWidgetArea; }
    virtual bool isClosable() const { return true; }
    virtual bool isFloatable() const { return true; }
};

#define IPLUGINWIDGET_IID "com.jxs.teacherMgrSys.IPluginWidget"
Q_DECLARE_INTERFACE(IPluginWidget, IPLUGINWIDGET_IID)

#endif // IPLUGINWIDGET_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pluginmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // 初始化插件管理器
    void initPluginManager();

private slots:
    // 插件窗口就绪槽
    void onPluginWidgetReady(const QString &pluginName, QWidget *widget);

private:
    Ui::MainWindow *ui;
    PluginManager *m_pluginManager;
};
#endif // MAINWINDOW_H

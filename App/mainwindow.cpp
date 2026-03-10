#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QDir>
#include "../../Plugins/ConfigUIPlugin/configuiplugin.h"
#include "../../Plugins/DatabaseUIPlugin/databaseuiplugin.h"
#include "../../Plugins/LogUIPlugin/loguiplugin.h"
#include "../../common/interfaces/IBasePlugin.h"
#include "../../common/interfaces/IConfigManager.h"
#include "../../common/interfaces/IDatabaseManager.h"
#include "../../common/interfaces/ILogManager.h"
#include "../../common/interfaces/IBaseEventBus.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_pluginManager(nullptr)
{
    ui->setupUi(this);
    initPluginManager();
}

MainWindow::~MainWindow()
{
    if (m_pluginManager) {
        delete m_pluginManager;
    }
    delete ui;
}

void MainWindow::initPluginManager()
{
    // 创建插件管理器
    m_pluginManager = new PluginManager(this);
    
    // 连接信号
    connect(m_pluginManager, &PluginManager::pluginWidgetReady, this, &MainWindow::onPluginWidgetReady);
    connect(m_pluginManager, &PluginManager::configManagerReady, this, &MainWindow::onConfigManagerReady);
    
    // 确定基础目录
    QString baseDir = QCoreApplication::applicationDirPath();
    
    // 优先使用 bin/x86_64 目录
    QString binDir = QDir::fromNativeSeparators("Q:/111/jxs_teacherMgrSys/bin/x86_64");
    if (QDir(binDir).exists()) {
        baseDir = binDir;
    }
    
    // 加载优先级配置
    QString priorityFile = baseDir + "/configs/priority.json";
    m_pluginManager->setPriorityFile(priorityFile);
    
    // 加载插件
    QString pluginsPath = baseDir + "/plugins";
    m_pluginManager->loadPlugins(pluginsPath);
}

void MainWindow::onPluginWidgetReady(const QString &pluginName, QWidget *widget)
{
    // 检查主窗口是否有 tabWidget
    QTabWidget *tabWidget = findChild<QTabWidget*>();
    if (!tabWidget) {
        // 如果没有 tabWidget，创建一个
        tabWidget = new QTabWidget(this);
        
        // 获取原来的 centralwidget
        QWidget *oldCentral = centralWidget();
        if (oldCentral) {
            // 删除原来的 centralwidget
            delete oldCentral;
        }
        
        // 设置新的 centralwidget
        setCentralWidget(tabWidget);
    }
    
    // 将插件窗口添加到 tabWidget
    tabWidget->addTab(widget, pluginName);

    // 如果是 ConfigUIPlugin，传递 configManager
    if (pluginName == "ConfigUIPlugin") {
        IBasePlugin *baseCore = m_pluginManager->getPlugin("BaseCorePlugin");
        IBasePlugin *configUI = m_pluginManager->getPlugin("ConfigUIPlugin");
        if (baseCore && configUI) {
            IConfigManager *configMgr = baseCore->configManager();
            if (configMgr) {
                QVariant var;
                var.setValue(static_cast<void*>(configMgr));
                configUI->setConfig("configManagerPtr", var);
            }
        }
    }

    // 如果是 DatabaseUIPlugin，传递 databaseManager
    if (pluginName == "DatabaseUIPlugin") {
        IBasePlugin *baseCore = m_pluginManager->getPlugin("BaseCorePlugin");
        IBasePlugin *dbUI = m_pluginManager->getPlugin("DatabaseUIPlugin");
        if (baseCore && dbUI) {
            IDatabaseManager *dbMgr = baseCore->databaseManager();
            if (dbMgr) {
                QVariant var;
                var.setValue(static_cast<void*>(dbMgr));
                dbUI->setConfig("databaseManagerPtr", var);
            }
        }
    }

    // 如果是 LogUIPlugin，传递 logManager
    if (pluginName == "LogUIPlugin") {
        IBasePlugin *baseCore = m_pluginManager->getPlugin("BaseCorePlugin");
        IBasePlugin *logUI = m_pluginManager->getPlugin("LogUIPlugin");
        if (baseCore && logUI) {
            ILogManager *logMgr = baseCore->logManager();
            if (logMgr) {
                QVariant var;
                var.setValue(static_cast<void*>(logMgr));
                logUI->setConfig("logManagerPtr", var);
            }
        }
    }

    // 如果是 EventUIPlugin，传递 eventManager
    if (pluginName == "EventUIPlugin") {
        IBasePlugin *baseCore = m_pluginManager->getPlugin("BaseCorePlugin");
        IBasePlugin *eventUI = m_pluginManager->getPlugin("EventUIPlugin");
        if (baseCore && eventUI) {
            IBaseEventBus *eventMgr = baseCore->eventManager();
            if (eventMgr) {
                QVariant var;
                var.setValue(static_cast<void*>(eventMgr));
                eventUI->setConfig("eventManagerPtr", var);
            }
        }
    }
    
    // 调整窗口大小
    resize(800, 600);
}

void MainWindow::onConfigManagerReady(IConfigManager *configManager)
{
    if (!configManager) {
        return;
    }

    IBasePlugin *configUI = m_pluginManager->getPlugin("ConfigUIPlugin");
    if (configUI) {
        QVariant var;
        var.setValue(static_cast<void*>(configManager));
        configUI->setConfig("configManagerPtr", var);
    }
}

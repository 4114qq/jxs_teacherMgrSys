#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QTabWidget>
#include <QDir>

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
    
    // 确定基础目录
    QString baseDir = QCoreApplication::applicationDirPath();
    
    // 优先使用 bin/x86_64 目录
    QString binDir = QDir::fromNativeSeparators("Q:/111/jxs_teacherMgrSys/bin/x86_64");
    if (QDir(binDir).exists()) {
        baseDir = binDir;
    }
    
    // 加载优先级配置
    QString priorityFile = baseDir + "/plugins/priority.json";
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
    
    // 调整窗口大小
    resize(800, 600);
}

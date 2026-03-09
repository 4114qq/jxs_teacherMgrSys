#include "App/pluginmanager.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 创建插件管理器
    PluginManager pluginManager;

    // 加载优先级配置文件
    QString priorityFile = QCoreApplication::applicationDirPath() + "/plugins/priority.json";
    pluginManager.setPriorityFile(priorityFile);

    // 设置默认优先级
    pluginManager.setDefaultPriority("BaseCorePlugin", 1);
    pluginManager.setDefaultPriority("ConfigUIPlugin", 2);
    pluginManager.setDefaultPriority("OtherPlugin", 3);

    // 测试拓扑排序
    QMap<QString, QStringList> testDependencies;
    testDependencies["ConfigUIPlugin"] = {"BaseCorePlugin"};
    testDependencies["OtherPlugin"] = {"BaseCorePlugin"};

    QStringList sorted = pluginManager.topologicalSort(testDependencies);
    qDebug() << "Topological sort result:" << sorted;

    // 测试按优先级排序
    QStringList prioritySorted = pluginManager.sortPluginsByPriority();
    qDebug() << "Priority sort result:" << prioritySorted;

    return 0;
}
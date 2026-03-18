#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include "../../common/interfaces/IConfigManager.h"
#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>

namespace Ui {
class ConfigWidget;
}

class ConfigModel;

class ConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigWidget(QWidget *parent = nullptr);
    ~ConfigWidget();

    void setConfigManager(IConfigManager *configManager);

private slots:
    void onSaveConfig();
    void onReloadConfig();
    void onAddGroup();
    void onAddConfig();
    void onDeleteConfig();

private:
    void initUI();

    Ui::ConfigWidget *ui;
    QTreeView *m_treeView;
    ConfigModel *m_model;
    IConfigManager *m_configManager;
};

#endif // CONFIGWIDGET_H

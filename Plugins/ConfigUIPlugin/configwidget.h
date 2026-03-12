#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include "../../common/interfaces/IConfigManager.h"
#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
class ConfigWidget;
}

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
    void onTreeItemChanged(QTreeWidgetItem *item, int column);

private:
    void initUI();
    void loadConfigToUI();

    Ui::ConfigWidget *ui;
    IConfigManager *m_configManager;
};

#endif // CONFIGWIDGET_H

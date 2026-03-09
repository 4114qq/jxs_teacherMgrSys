#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include "../../common/interfaces/IConfigManager.h"
#include <QWidget>
#include <QTableWidgetItem>

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
    void onAddConfig();
    void onEditConfig();
    void onDeleteConfig();
    void onReloadConfig();
    void onTableItemChanged(QTableWidgetItem *item);

private:
    void initUI();
    void loadConfigToUI();
    void saveUIToConfig();

    Ui::ConfigWidget *ui;
    IConfigManager *m_configManager;
    bool m_isEditing;
};

#endif // CONFIGWIDGET_H

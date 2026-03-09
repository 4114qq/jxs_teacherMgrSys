#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include "../../common/interfaces/IConfigManager.h"
#include <QWidget>

namespace Ui {
class ConfigWidget;
}

/**
 * @brief 配置管理窗口
 * 用于显示和编辑配置项
 */
class ConfigWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigWidget(QWidget *parent = nullptr);
    ~ConfigWidget();

    // 设置配置管理器
    void setConfigManager(IConfigManager *configManager);

private slots:
    // 保存配置
    void onSaveConfig();
    // 加载配置
    void onLoadConfig();
    // 重置配置
    void onResetConfig();

private:
    Ui::ConfigWidget *ui;
    IConfigManager *m_configManager;

    // 初始化界面
    void initUI();
    // 加载配置到界面
    void loadConfigToUI();
    // 保存界面配置
    void saveUIToConfig();
};

#endif // CONFIGWIDGET_H

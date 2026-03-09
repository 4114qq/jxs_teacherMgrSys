#include "configwidget.h"
#include "ui_configwidget.h"
#include "../../common/interfaces/IConfigManager.h"
#include <QMessageBox>
#include <QPushButton>

ConfigWidget::ConfigWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConfigWidget)
    , m_configManager(nullptr)
{
    ui->setupUi(this);
    initUI();
}

ConfigWidget::~ConfigWidget()
{
    delete ui;
}

void ConfigWidget::setConfigManager(IConfigManager *configManager)
{
    m_configManager = configManager;
    if (m_configManager) {
        loadConfigToUI();
    }
}

void ConfigWidget::initUI()
{
    // 初始化界面组件
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ConfigWidget::onSaveConfig);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ConfigWidget::close);
    
    // 重置按钮
    QPushButton *resetButton = ui->buttonBox->button(QDialogButtonBox::Reset);
    if (resetButton) {
        connect(resetButton, &QPushButton::clicked, this, &ConfigWidget::onResetConfig);
    }
}

void ConfigWidget::loadConfigToUI()
{
    if (!m_configManager) {
        return;
    }

    // 加载通用配置
    QString language = m_configManager->get("general/language", "中文").toString();
    QString theme = m_configManager->get("general/theme", "默认").toString();
    bool autoSave = m_configManager->get("general/autoSave", false).toBool();

    ui->comboBoxLanguage->setCurrentText(language);
    ui->comboBoxTheme->setCurrentText(theme);
    ui->checkBoxAutoSave->setChecked(autoSave);

    // 加载数据库配置
    QString host = m_configManager->get("database/host", "localhost").toString();
    int port = m_configManager->get("database/port", 3306).toInt();
    QString database = m_configManager->get("database/name", "teacher_mgr").toString();
    QString username = m_configManager->get("database/username", "root").toString();
    QString password = m_configManager->get("database/password", "").toString();

    ui->lineEditHost->setText(host);
    ui->lineEditPort->setText(QString::number(port));
    ui->lineEditDatabase->setText(database);
    ui->lineEditUsername->setText(username);
    ui->lineEditPassword->setText(password);
}

void ConfigWidget::saveUIToConfig()
{
    if (!m_configManager) {
        return;
    }

    // 保存通用配置
    m_configManager->set("general/language", ui->comboBoxLanguage->currentText());
    m_configManager->set("general/theme", ui->comboBoxTheme->currentText());
    m_configManager->set("general/autoSave", ui->checkBoxAutoSave->isChecked());

    // 保存数据库配置
    m_configManager->set("database/host", ui->lineEditHost->text());
    m_configManager->set("database/port", ui->lineEditPort->text().toInt());
    m_configManager->set("database/name", ui->lineEditDatabase->text());
    m_configManager->set("database/username", ui->lineEditUsername->text());
    m_configManager->set("database/password", ui->lineEditPassword->text());
}

void ConfigWidget::onSaveConfig()
{
    if (!m_configManager) {
        QMessageBox::warning(this, "警告", "配置管理器未初始化");
        return;
    }

    saveUIToConfig();
    if (m_configManager->save()) {
        QMessageBox::information(this, "成功", "配置保存成功");
    } else {
        QMessageBox::critical(this, "错误", "配置保存失败");
    }
}

void ConfigWidget::onLoadConfig()
{
    if (!m_configManager) {
        QMessageBox::warning(this, "警告", "配置管理器未初始化");
        return;
    }

    if (m_configManager->load()) {
        loadConfigToUI();
        QMessageBox::information(this, "成功", "配置加载成功");
    } else {
        QMessageBox::critical(this, "错误", "配置加载失败");
    }
}

void ConfigWidget::onResetConfig()
{
    if (!m_configManager) {
        QMessageBox::warning(this, "警告", "配置管理器未初始化");
        return;
    }

    if (QMessageBox::question(this, "确认", "确定要重置配置吗？") == QMessageBox::Yes) {
        // 重置为默认值
        m_configManager->set("general/language", "中文");
        m_configManager->set("general/theme", "默认");
        m_configManager->set("general/autoSave", false);
        m_configManager->set("database/host", "localhost");
        m_configManager->set("database/port", 3306);
        m_configManager->set("database/name", "teacher_mgr");
        m_configManager->set("database/username", "root");
        m_configManager->set("database/password", "");
        
        m_configManager->save();
        loadConfigToUI();
        QMessageBox::information(this, "成功", "配置已重置为默认值");
    }
}

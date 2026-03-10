#include "configwidget.h"
#include "ui_configwidget.h"
#include "../../common/interfaces/IConfigManager.h"
#include <QMessageBox>
#include <QPushButton>
#include <QInputDialog>
#include <QHeaderView>

ConfigWidget::ConfigWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConfigWidget)
    , m_configManager(nullptr)
    , m_isEditing(false)
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
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("Key") << QString::fromLocal8Bit("Value") << QString::fromLocal8Bit("Description"));
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);

    connect(ui->btnAdd, &QPushButton::clicked, this, &ConfigWidget::onAddConfig);
    connect(ui->btnEdit, &QPushButton::clicked, this, &ConfigWidget::onEditConfig);
    connect(ui->btnDelete, &QPushButton::clicked, this, &ConfigWidget::onDeleteConfig);
    connect(ui->btnReload, &QPushButton::clicked, this, &ConfigWidget::onReloadConfig);
    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &ConfigWidget::onTableItemChanged);
}

void ConfigWidget::loadConfigToUI()
{
    if (!m_configManager) {
        return;
    }

    ui->tableWidget->blockSignals(true);
    ui->tableWidget->setRowCount(0);

    QList<ConfigItem> items = m_configManager->getConfigItems();
    for (const ConfigItem &item : items) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        QTableWidgetItem *keyItem = new QTableWidgetItem(item.key);
        keyItem->setFlags(keyItem->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 0, keyItem);

        QString valueStr;
        if (item.type == "bool") {
            valueStr = item.value.toBool() ? "true" : "false";
        } else if (item.type == "int" || item.type == "double") {
            valueStr = item.value.toString();
        } else {
            valueStr = item.value.toString();
        }
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(valueStr));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(item.description));
    }

    ui->tableWidget->blockSignals(false);
}

void ConfigWidget::saveUIToConfig()
{
    if (!m_configManager) {
        return;
    }

    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        ConfigItem item;
        item.key = ui->tableWidget->item(row, 0)->text();
        item.description = ui->tableWidget->item(row, 2)->text();

        QString valueStr = ui->tableWidget->item(row, 1)->text();

        if (valueStr.toLower() == "true" || valueStr.toLower() == "false") {
            item.value = (valueStr.toLower() == "true");
            item.type = "bool";
        } else {
            bool ok;
            int intValue = valueStr.toInt(&ok);
            if (ok) {
                item.value = intValue;
                item.type = "int";
            } else {
                double doubleValue = valueStr.toDouble(&ok);
                if (ok) {
                    item.value = doubleValue;
                    item.type = "double";
                } else {
                    item.value = valueStr;
                    item.type = "string";
                }
            }
        }

        m_configManager->updateConfigItem(item);
    }
}

void ConfigWidget::onSaveConfig()
{
    if (!m_configManager) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("配置管理器未初始化"));
        return;
    }

    saveUIToConfig();
    if (m_configManager->save()) {
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("配置保存成功"));
    } else {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("配置保存失败"));
    }
}

void ConfigWidget::onAddConfig()
{
    if (!m_configManager) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("配置管理器未初始化"));
        return;
    }

    bool ok;
    QString key = QInputDialog::getText(this, QString::fromLocal8Bit("添加配置"), QString::fromLocal8Bit("Key:"), QLineEdit::Normal, "", &ok);
    if (!ok || key.isEmpty()) {
        return;
    }

    if (m_configManager->contains(key)) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("Key 已存在"));
        return;
    }

    QString value = QInputDialog::getText(this, QString::fromLocal8Bit("添加配置"), QString::fromLocal8Bit("Value:"), QLineEdit::Normal, "", &ok);
    if (!ok) {
        return;
    }

    QString description = QInputDialog::getText(this, QString::fromLocal8Bit("添加配置"), QString::fromLocal8Bit("Description (描述):"), QLineEdit::Normal, "", &ok);
    if (!ok) {
        return;
    }

    ConfigItem item;
    item.key = key;
    item.description = description;

    if (value.toLower() == "true" || value.toLower() == "false") {
        item.value = (value.toLower() == "true");
        item.type = "bool";
    } else {
        bool isInt;
        int intValue = value.toInt(&isInt);
        if (isInt) {
            item.value = intValue;
            item.type = "int";
        } else {
            bool isDouble;
            double doubleValue = value.toDouble(&isDouble);
            if (isDouble) {
                item.value = doubleValue;
                item.type = "double";
            } else {
                item.value = value;
                item.type = "string";
            }
        }
    }

    m_configManager->addConfigItem(item);
    loadConfigToUI();
}

void ConfigWidget::onEditConfig()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("请选择要编辑的行"));
        return;
    }

    ui->tableWidget->editItem(ui->tableWidget->item(currentRow, 1));
}

void ConfigWidget::onDeleteConfig()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, QString::fromLocal8Bit("提示"), QString::fromLocal8Bit("请选择要删除的行"));
        return;
    }

    QString key = ui->tableWidget->item(currentRow, 0)->text();
    if (QMessageBox::question(this, QString::fromLocal8Bit("确认"), QString::fromLocal8Bit("确定要删除配置项 [%1] 吗？").arg(key)) == QMessageBox::Yes) {
        m_configManager->deleteConfigItem(key);
        loadConfigToUI();
    }
}

void ConfigWidget::onReloadConfig()
{
    if (!m_configManager) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("配置管理器未初始化"));
        return;
    }

    if (m_configManager->reload()) {
        loadConfigToUI();
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("配置重新加载成功"));
    } else {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("配置重新加载失败"));
    }
}

void ConfigWidget::onTableItemChanged(QTableWidgetItem *item)
{
    if (!m_configManager || m_isEditing) {
        return;
    }

    int row = item->row();
    int col = item->column();
    if (col != 1) {
        return;
    }

    QString key = ui->tableWidget->item(row, 0)->text();
    QString value = item->text();
    QString description = ui->tableWidget->item(row, 2)->text();

    ConfigItem configItem;
    configItem.key = key;
    configItem.description = description;

    if (value.toLower() == "true" || value.toLower() == "false") {
        configItem.value = (value.toLower() == "true");
        configItem.type = "bool";
    } else {
        bool isInt;
        int intValue = value.toInt(&isInt);
        if (isInt) {
            configItem.value = intValue;
            configItem.type = "int";
        } else {
            bool isDouble;
            double doubleValue = value.toDouble(&isDouble);
            if (isDouble) {
                configItem.value = doubleValue;
                configItem.type = "double";
            } else {
                configItem.value = value;
                configItem.type = "string";
            }
        }
    }

    m_configManager->updateConfigItem(configItem);
}

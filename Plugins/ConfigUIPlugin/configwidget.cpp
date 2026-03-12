#include "configwidget.h"
#include "ui_configwidget.h"
#include "../../common/interfaces/IConfigManager.h"
#include <QMessageBox>
#include <QPushButton>
#include <QInputDialog>

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
    ui->treeWidget->setColumnCount(3);
    ui->treeWidget->setHeaderLabels(QStringList() << QString::fromLocal8Bit("配置项") << QString::fromLocal8Bit("值") << QString::fromLocal8Bit("描述"));
    ui->treeWidget->header()->setStretchLastSection(true);
    ui->treeWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->treeWidget->setAlternatingRowColors(true);

    connect(ui->btnReload, &QPushButton::clicked, this, &ConfigWidget::onReloadConfig);
    connect(ui->btnAddGroup, &QPushButton::clicked, this, &ConfigWidget::onAddGroup);
    connect(ui->btnAddConfig, &QPushButton::clicked, this, &ConfigWidget::onAddConfig);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &ConfigWidget::onSaveConfig);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ConfigWidget::close);
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, &ConfigWidget::onTreeItemChanged);
}

void ConfigWidget::loadConfigToUI()
{
    if (!m_configManager) {
        return;
    }

    ui->treeWidget->blockSignals(true);
    ui->treeWidget->clear();

    QList<ConfigItem> items = m_configManager->getConfigItems();
    QMap<QString, QTreeWidgetItem*> groupMap;

    for (const ConfigItem &item : items) {
        QString groupName = item.group.isEmpty() ? "General" : item.group;

        QTreeWidgetItem *groupItem;
        if (groupMap.contains(groupName)) {
            groupItem = groupMap[groupName];
        } else {
            groupItem = new QTreeWidgetItem(ui->treeWidget);
            groupItem->setText(0, groupName);
            groupItem->setFlags(groupItem->flags() & ~Qt::ItemIsEditable);
            groupMap[groupName] = groupItem;
            ui->treeWidget->addTopLevelItem(groupItem);
        }

        QTreeWidgetItem *childItem = new QTreeWidgetItem(groupItem);
        childItem->setText(0, item.key);
        childItem->setFlags(childItem->flags() | Qt::ItemIsEditable);

        QString valueStr;
        if (item.type == "bool") {
            valueStr = item.value.toBool() ? "true" : "false";
        } else if (item.type == "int" || item.type == "double") {
            valueStr = item.value.toString();
        } else {
            valueStr = item.value.toString();
        }
        childItem->setText(1, valueStr);
        childItem->setText(2, item.description);

        childItem->setData(0, Qt::UserRole, item.key);
        childItem->setData(1, Qt::UserRole, item.type);
        childItem->setData(2, Qt::UserRole, item.group);

        groupItem->addChild(childItem);
    }

    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        ui->treeWidget->topLevelItem(i)->setExpanded(true);
    }

    ui->treeWidget->blockSignals(false);
}

void ConfigWidget::onSaveConfig()
{
    if (!m_configManager) {
        return;
    }

    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem *groupItem = ui->treeWidget->topLevelItem(i);
        QString groupName = groupItem->text(0);

        for (int j = 0; j < groupItem->childCount(); ++j) {
            QTreeWidgetItem *item = groupItem->child(j);
            QString key = item->data(0, Qt::UserRole).toString();
            if (key.isEmpty()) {
                key = item->text(0);
            }
            if (key.isEmpty()) {
                continue;
            }
            QString value = item->text(1);
            QString description = item->text(2);
            QString actualGroup = groupName == "General" ? "" : groupName;

            ConfigItem configItem;
            configItem.key = key;
            configItem.group = actualGroup;
            configItem.value = value;
            configItem.description = description;
            configItem.type = item->data(1, Qt::UserRole).toString();
            m_configManager->updateConfigItem(configItem);
        }
    }

    if (m_configManager->save()) {
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("配置保存成功!"));
    } else {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("配置保存失败!"));
    }
}

void ConfigWidget::onReloadConfig()
{
    if (m_configManager->reload()) {
        loadConfigToUI();
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("配置重新加载成功!"));
    } else {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("配置重新加载失败!"));
    }
}

void ConfigWidget::onTreeItemChanged(QTreeWidgetItem *item, int column)
{
    if (!item || !item->parent()) {
        return;
    }

    QString key = item->data(0, Qt::UserRole).toString();
    QString group = item->data(2, Qt::UserRole).toString();

    if (!m_configManager) {
        return;
    }

    ConfigItem configItem;
    configItem.key = key;
    configItem.group = group;

    if (column == 1) {
        QString value = item->text(1);
        configItem.value = value;
    } else if (column == 2) {
        QString description = item->text(2);
        configItem.description = description;
        QList<ConfigItem> items = m_configManager->getConfigItems();
        for (const auto &existing : items) {
            if (existing.key == key && existing.group == group) {
                configItem.value = existing.value;
                configItem.type = existing.type;
                break;
            }
        }
    }

    m_configManager->updateConfigItem(configItem);
}

void ConfigWidget::onAddGroup()
{
    bool ok;
    QString groupName = QInputDialog::getText(this, QString::fromLocal8Bit("添加分组"), QString::fromLocal8Bit("分组名称:"), QLineEdit::Normal, "", &ok);
    if (!ok || groupName.isEmpty()) {
        return;
    }

    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        if (ui->treeWidget->topLevelItem(i)->text(0) == groupName) {
            QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("分组已存在!"));
            return;
        }
    }

    QTreeWidgetItem *groupItem = new QTreeWidgetItem(ui->treeWidget);
    groupItem->setText(0, groupName);
    groupItem->setFlags(groupItem->flags() & ~Qt::ItemIsEditable);
    ui->treeWidget->addTopLevelItem(groupItem);
    groupItem->setExpanded(true);
}

void ConfigWidget::onAddConfig()
{
    bool ok;
    QString key = QInputDialog::getText(this, QString::fromLocal8Bit("添加配置"), QString::fromLocal8Bit("配置项:"), QLineEdit::Normal, "", &ok);
    if (!ok || key.isEmpty()) {
        return;
    }

    QString value = QInputDialog::getText(this, QString::fromLocal8Bit("添加配置"), QString::fromLocal8Bit("值:"), QLineEdit::Normal, "", &ok);
    if (!ok) {
        return;
    }

    QString groupName = "General";
    QTreeWidgetItem *selectedItem = ui->treeWidget->currentItem();
    if (selectedItem) {
        if (selectedItem->parent()) {
            groupName = selectedItem->parent()->text(0);
        } else {
            groupName = selectedItem->text(0);
        }
    }

    QTreeWidgetItem *groupItem = nullptr;
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
        if (ui->treeWidget->topLevelItem(i)->text(0) == groupName) {
            groupItem = ui->treeWidget->topLevelItem(i);
            break;
        }
    }

    if (!groupItem) {
        groupItem = new QTreeWidgetItem(ui->treeWidget);
        groupItem->setText(0, groupName);
        groupItem->setFlags(groupItem->flags() & ~Qt::ItemIsEditable);
        ui->treeWidget->addTopLevelItem(groupItem);
    }

    QTreeWidgetItem *childItem = new QTreeWidgetItem(groupItem);
    childItem->setText(0, key);
    childItem->setText(1, value);
    childItem->setFlags(childItem->flags() | Qt::ItemIsEditable);
    childItem->setData(0, Qt::UserRole, key);
    childItem->setData(1, Qt::UserRole, "string");
    childItem->setData(2, Qt::UserRole, groupName == "General" ? "" : groupName);
    groupItem->addChild(childItem);
    groupItem->setExpanded(true);
}

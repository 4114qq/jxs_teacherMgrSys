#include "configwidget.h"
#include "ui_configwidget.h"
#include "configmodel.h"
#include "addconfigdialog.h"
#include "../../common/interfaces/IConfigManager.h"
#include <QMessageBox>
#include <QPushButton>
#include <QInputDialog>
#include <QStandardItem>
#include <QHeaderView>
#include <QSizePolicy>

ConfigWidget::ConfigWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConfigWidget)
    , m_model(nullptr)
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
    if (m_model && m_configManager) {
        m_model->setConfigManager(m_configManager);
        m_model->loadConfig();
    }
}

void ConfigWidget::initUI()
{
    m_model = new ConfigModel(this);
    m_treeView = ui->treeView;
    m_treeView->setModel(m_model);
    m_treeView->setAlternatingRowColors(true);
    m_treeView->setEditTriggers(QAbstractItemView::DoubleClicked);
    m_treeView->setHeaderHidden(false);
    m_treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_model->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("配置项") << QString::fromLocal8Bit("值") << QString::fromLocal8Bit("描述"));

    m_treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_treeView->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_treeView->header()->setSectionResizeMode(2, QHeaderView::Stretch);

    m_treeView->setDragEnabled(true);
    m_treeView->setAcceptDrops(true);
    m_treeView->setDropIndicatorShown(true);
    m_treeView->setDragDropMode(QAbstractItemView::DragDrop);
    m_treeView->setDefaultDropAction(Qt::MoveAction);
    m_treeView->setItemsExpandable(true);
    m_treeView->expandAll();

    connect(ui->btnReload, &QPushButton::clicked, this, &ConfigWidget::onReloadConfig);
    connect(ui->btnAddGroup, &QPushButton::clicked, this, &ConfigWidget::onAddGroup);
    connect(ui->btnAddConfig, &QPushButton::clicked, this, &ConfigWidget::onAddConfig);
    connect(ui->btnDelete, &QPushButton::clicked, this, &ConfigWidget::onDeleteConfig);
    connect(ui->btnSave, &QPushButton::clicked, this, &ConfigWidget::onSaveConfig);
}

void ConfigWidget::onSaveConfig()
{
    if (m_model) {
        m_model->saveConfig();
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("配置保存成功!"));
    }
}

void ConfigWidget::onReloadConfig()
{
    if (m_configManager && m_configManager->reload()) {
        if (m_model) {
            m_model->loadConfig();
        }
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("配置重新加载成功!"));
    } else {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("配置重新加载失败!"));
    }
}

void ConfigWidget::onAddGroup()
{
    bool ok;
    QString groupName = QInputDialog::getText(this, QString::fromLocal8Bit("添加分组"), QString::fromLocal8Bit("分组名称:"), QLineEdit::Normal, "", &ok);
    if (!ok || groupName.isEmpty()) {
        return;
    }

    if (m_model) {
        for (int i = 0; i < m_model->rowCount(); ++i) {
            if (m_model->item(i, 0)->text() == groupName) {
                QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("分组已存在!"));
                return;
            }
        }

        QStandardItem *groupItem = new QStandardItem(groupName);
        groupItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDropEnabled);
        m_model->appendRow(groupItem);
    }
}

void ConfigWidget::onAddConfig()
{
    QStringList groups;
    if (m_model) {
        for (int i = 0; i < m_model->rowCount(); ++i) {
            groups.append(m_model->item(i, 0)->text());
        }
    }

    AddConfigDialog dialog(groups, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString key = dialog.key();
        QString value = dialog.value();
        QString description = dialog.description();
        QString groupName = dialog.group();
        if (groupName.isEmpty()) {
            groupName = "General";
        }

        QStandardItem *groupItem = nullptr;
        if (m_model) {
            for (int i = 0; i < m_model->rowCount(); ++i) {
                if (m_model->item(i, 0)->text() == groupName) {
                    groupItem = m_model->item(i, 0);
                    break;
                }
            }

            if (!groupItem) {
                groupItem = new QStandardItem(groupName);
                m_model->appendRow(groupItem);
            }

            QStandardItem *keyItem = new QStandardItem(key);
            keyItem->setData("string", Qt::UserRole);
            keyItem->setData(key, Qt::UserRole + 1);

            QStandardItem *valueItem = new QStandardItem(value);
            QStandardItem *descItem = new QStandardItem(description);

            QList<QStandardItem*> rowItems;
            rowItems << keyItem << valueItem << descItem;
            groupItem->appendRow(rowItems);
        }
    }
}

void ConfigWidget::onDeleteConfig()
{
    QModelIndex currentIndex = m_treeView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请先选择要删除的项!"));
        return;
    }

    QStandardItem *item = m_model->itemFromIndex(currentIndex);
    if (!item) {
        return;
    }

    if (item->parent()) {
        int ret = QMessageBox::question(this, QString::fromLocal8Bit("确认"),
            QString::fromLocal8Bit("确定要删除配置项 \"%1\" 吗?").arg(item->text()),
            QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            QStandardItem *parent = item->parent();
            parent->removeRow(item->row());
            if (m_configManager) {
                m_configManager->save();
            }
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("删除成功!"));
        }
    } else {
        int ret = QMessageBox::question(this, QString::fromLocal8Bit("确认"),
            QString::fromLocal8Bit("确定要删除分组 \"%1\" 及其所有配置项吗?").arg(item->text()),
            QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            m_model->removeRow(item->row());
            if (m_configManager) {
                m_configManager->save();
            }
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("删除成功!"));
        }
    }
}

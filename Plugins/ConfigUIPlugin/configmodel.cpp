#include "configmodel.h"
#include "../common/interfaces/IConfigManager.h"
#include <QStandardItem>
#include <QDataStream>
#include <QTextStream>

ConfigModel::ConfigModel(QObject *parent)
    : QStandardItemModel(parent)
    , m_configManager(nullptr)
{
    setColumnCount(3);
    setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("配置项") << QString::fromLocal8Bit("值") << QString::fromLocal8Bit("描述"));
}

void ConfigModel::setConfigManager(void *configManager)
{
    m_configManager = configManager;
}

Qt::ItemFlags ConfigModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::ItemIsDropEnabled;
    }

    Qt::ItemFlags flags = QStandardItemModel::flags(index);

    QStandardItem *item = itemFromIndex(index);
    bool isChild = item && item->parent();

    if (isChild) {
        flags |= Qt::ItemIsDragEnabled | Qt::ItemIsEditable;
    } else {
        flags &= ~Qt::ItemIsDragEnabled;
        flags &= ~Qt::ItemIsEditable;
        flags |= Qt::ItemIsDropEnabled;
    }

    return flags;
}

QStringList ConfigModel::mimeTypes() const
{
    return QStringList() << "application/x-configitem";
}

QMimeData *ConfigModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    for (const QModelIndex &index : indexes) {
        if (index.isValid() && index.column() == 0) {
            QStandardItem *item = itemFromIndex(index);
            if (item && item->parent()) {
                encodedData.append(QString::number(item->index().row()).toUtf8());
                encodedData.append('|');
                encodedData.append(QString::number(item->parent()->index().row()).toUtf8());
                encodedData.append('|');
                encodedData.append(item->text().toUtf8());
                QModelIndex valueIndex = index.sibling(index.row(), 1);
                QModelIndex descIndex = index.sibling(index.row(), 2);
                encodedData.append('|');
                encodedData.append(valueIndex.isValid() ? valueIndex.data().toString().toUtf8() : QByteArray());
                encodedData.append('|');
                encodedData.append(descIndex.isValid() ? descIndex.data().toString().toUtf8() : QByteArray());
                encodedData.append('|');
                encodedData.append(item->data(Qt::UserRole).toString().toUtf8());
                encodedData.append('\n');
            }
        }
    }

    mimeData->setData("application/x-configitem", encodedData);
    return mimeData;
}

bool ConfigModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(action);
    Q_UNUSED(row);
    Q_UNUSED(column);

    if (!data->hasFormat("application/x-configitem")) {
        return false;
    }

    if (!parent.isValid()) {
        return false;
    }

    if (parent.parent().isValid()) {
        return false;
    }

    QStandardItem *targetGroup = itemFromIndex(parent);
    if (!targetGroup) {
        return false;
    }
    QString targetGroupName = targetGroup->text();

    QByteArray encodedData = data->data("application/x-configitem");
    QTextStream stream(&encodedData, QIODevice::ReadOnly);

    struct MoveInfo {
        int sourceRow;
        int sourceParentRow;
        QString key;
        QString value;
        QString description;
        QString type;
    };
    QList<MoveInfo> moveList;

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList parts = line.split('|');
        if (parts.size() < 6) continue;

        MoveInfo info;
        info.sourceRow = parts[0].toInt();
        info.sourceParentRow = parts[1].toInt();
        info.key = parts[2];
        info.value = parts[3];
        info.description = parts[4];
        info.type = parts[5];
        moveList.append(info);
    }

    for (int i = moveList.size() - 1; i >= 0; --i) {
        const MoveInfo &info = moveList[i];
        QStandardItem *sourceParent = this->item(info.sourceParentRow);
        if (!sourceParent) continue;

        QString sourceGroupName = sourceParent->text();
        if (sourceGroupName == targetGroupName) {
            continue;
        }

        QStandardItem *keyItem = sourceParent->child(info.sourceRow, 0);
        QStandardItem *valueItem = sourceParent->child(info.sourceRow, 1);
        QStandardItem *descItem = sourceParent->child(info.sourceRow, 2);

        if (!keyItem) continue;

        QString key = keyItem->text();
        QString value = valueItem ? valueItem->text() : QString();
        QString description = descItem ? descItem->text() : QString();
        QString type = keyItem->data(Qt::UserRole).toString();

        sourceParent->removeRow(info.sourceRow);

        QStandardItem *newKeyItem = new QStandardItem(info.key);
        newKeyItem->setData(info.type, Qt::UserRole);
        newKeyItem->setData(info.key, Qt::UserRole + 1);

        QStandardItem *newValueItem = new QStandardItem(value);
        QStandardItem *newDescItem = new QStandardItem(description);

        QList<QStandardItem*> rowItems;
        rowItems << newKeyItem << newValueItem << newDescItem;
        targetGroup->appendRow(rowItems);
    }

    return true;
}

Qt::DropActions ConfigModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

void ConfigModel::loadConfig()
{
    if (!m_configManager) return;

    clear();
    setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("配置项") << QString::fromLocal8Bit("值") << QString::fromLocal8Bit("描述"));

    IConfigManager *configManager = static_cast<IConfigManager*>(m_configManager);
    QList<ConfigItem> items = configManager->getConfigItems();
    QMap<QString, QStandardItem*> groupMap;

    for (const ConfigItem &item : items) {
        QString groupName = item.group.isEmpty() ? "General" : item.group;

        QStandardItem *groupItem;
        if (groupMap.contains(groupName)) {
            groupItem = groupMap[groupName];
        } else {
            groupItem = new QStandardItem(groupName);
            groupMap[groupName] = groupItem;
            appendRow(groupItem);
        }

        QStandardItem *keyItem = new QStandardItem(item.key);
        keyItem->setData(item.type, Qt::UserRole);
        keyItem->setData(item.key, Qt::UserRole + 1);

        QString valueStr;
        if (item.type == "bool") {
            valueStr = item.value.toBool() ? "true" : "false";
        } else {
            valueStr = item.value.toString();
        }

        QStandardItem *valueItem = new QStandardItem(valueStr);

        QStandardItem *descItem = new QStandardItem(item.description);

        QList<QStandardItem*> rowItems;
        rowItems << keyItem << valueItem << descItem;
        groupItem->appendRow(rowItems);
    }
}

void ConfigModel::saveConfig()
{
    if (!m_configManager) return;

    IConfigManager *configManager = static_cast<IConfigManager*>(m_configManager);

    configManager->clear();

    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *groupItem = item(i, 0);
        QString groupName = groupItem->text();
        QString actualGroup = groupName == "General" ? "" : groupName;

        for (int j = 0; j < groupItem->rowCount(); ++j) {
            QStandardItem *keyItem = groupItem->child(j, 0);
            QStandardItem *valueItem = groupItem->child(j, 1);
            QStandardItem *descItem = groupItem->child(j, 2);

            ConfigItem configItem;
            configItem.key = keyItem->data(Qt::UserRole + 1).toString();
            if (configItem.key.isEmpty()) {
                configItem.key = keyItem->text();
            }
            configItem.group = actualGroup;
            configItem.value = valueItem ? valueItem->text() : QString();
            configItem.description = descItem ? descItem->text() : QString();
            configItem.type = keyItem->data(Qt::UserRole).toString();

            configManager->addConfigItem(configItem);
        }
    }

    configManager->save();
}

#include "largestepmodel.h"
#include <QUuid>

LargeStepModel::LargeStepModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_selectedIndex(-1)
    , m_isMoving(false)
{
}

LargeStepModel::~LargeStepModel()
{
}

int LargeStepModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_steps.size() + 1;
}

QVariant LargeStepModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int stepCount = m_steps.size();

    if (row == stepCount) {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return QString::fromLocal8Bit("点击添加步骤");
        if (role == Qt::UserRole + 2)
            return true;
        return QVariant();
    }

    if (row > stepCount)
        return QVariant();

    const LargeStepItem &step = m_steps.at(row);

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return step.name;
    case Qt::UserRole:
        return step.id;
    case Qt::UserRole + 1:
        return step.order;
    case Qt::UserRole + 2:
        return false;
    default:
        return QVariant();
    }
}

bool LargeStepModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    int row = index.row();
    int stepCount = m_steps.size();

    if (row == stepCount) {
        if (role == Qt::EditRole) {
            QString name = value.toString();
            if (!name.isEmpty()) {
                addStep(name);
            }
            return true;
        }
        return false;
    }

    if (row > stepCount)
        return false;

    if (role == Qt::EditRole) {
        m_steps[row].name = value.toString();
        emit dataChanged(index, index);
        emit stepsChanged();
        return true;
    }

    return false;
}

Qt::ItemFlags LargeStepModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsDropEnabled;

    int row = index.row();
    if (row >= m_steps.size()) {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return QAbstractItemModel::flags(index) |
           Qt::ItemIsDragEnabled |
           Qt::ItemIsDropEnabled |
           Qt::ItemIsEditable;
}

bool LargeStepModel::isAddItem(int index) const
{
    return index == m_steps.size();
}

int LargeStepModel::addItemIndex() const
{
    return m_steps.size();
}

Qt::DropActions LargeStepModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool LargeStepModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                              const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent != destinationParent || count != 1 || m_isMoving)
        return false;

    int stepCount = m_steps.size();

    if (sourceRow >= stepCount || destinationChild > stepCount)
        return false;

    if (sourceRow == destinationChild)
        return true;

    int destRow = destinationChild;
    if (destinationChild > sourceRow) {
        destRow = destinationChild - 1;
    }

    if (destRow >= stepCount)
        destRow = stepCount - 1;

    if (sourceRow == destRow)
        return true;

    m_isMoving = true;
    beginResetModel();

    m_steps.move(sourceRow, destRow);

    for (int i = 0; i < m_steps.size(); ++i) {
        m_steps[i].order = i + 1;
    }

    endResetModel();
    m_isMoving = false;
    emit stepsChanged();
    return true;
}

void LargeStepModel::addStep(const QString &name)
{
    beginInsertRows(QModelIndex(), m_steps.size(), m_steps.size());

    LargeStepItem item;
    item.id = QUuid::createUuid().toString();
    item.name = name;
    item.order = m_steps.size() + 1;

    m_steps.append(item);
    endInsertRows();
    emit stepsChanged();
}

void LargeStepModel::insertStep(int index, const QString &name)
{
    if (index < 0 || index > m_steps.size())
        index = m_steps.size();

    beginInsertRows(QModelIndex(), index, index);

    LargeStepItem item;
    item.id = QUuid::createUuid().toString();
    item.name = name;
    item.order = index + 1;

    m_steps.insert(index, item);

    for (int i = index + 1; i < m_steps.size(); ++i) {
        m_steps[i].order = i + 1;
    }

    endInsertRows();
    emit stepsChanged();
}

void LargeStepModel::removeStep(int index)
{
    if (index < 0 || index >= m_steps.size())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_steps.removeAt(index);
    endRemoveRows();

    for (int i = index; i < m_steps.size(); ++i) {
        m_steps[i].order = i + 1;
    }

    if (m_selectedIndex >= m_steps.size()) {
        m_selectedIndex = m_steps.isEmpty() ? -1 : m_steps.size() - 1;
    }

    emit stepsChanged();
}

void LargeStepModel::moveStep(int from, int to)
{
    if (from < 0 || from >= m_steps.size() ||
        to < 0 || to >= m_steps.size() ||
        from == to)
        return;

    beginResetModel();
    m_steps.move(from, to);

    for (int i = 0; i < m_steps.size(); ++i) {
        m_steps[i].order = i + 1;
    }

    endResetModel();
    emit stepsChanged();
}

void LargeStepModel::renameStep(int index, const QString &name)
{
    if (index < 0 || index >= m_steps.size())
        return;

    m_steps[index].name = name;
    QModelIndex idx = createIndex(index, 0);
    emit dataChanged(idx, idx);
    emit stepsChanged();
}

void LargeStepModel::clear()
{
    beginResetModel();
    m_steps.clear();
    m_selectedIndex = -1;
    endResetModel();
    emit stepsChanged();
}

LargeStepItem LargeStepModel::stepAt(int index) const
{
    if (index < 0 || index >= m_steps.size())
        return LargeStepItem();
    return m_steps.at(index);
}

bool LargeStepModel::isEmpty() const
{
    return m_steps.isEmpty();
}

void LargeStepModel::setSelectedIndex(int index)
{
    if (index == m_selectedIndex)
        return;

    int oldIndex = m_selectedIndex;
    m_selectedIndex = index;

    if (oldIndex >= 0 && oldIndex < m_steps.size()) {
        emit dataChanged(createIndex(oldIndex, 0), createIndex(oldIndex, 0));
    }
    if (m_selectedIndex >= 0 && m_selectedIndex < m_steps.size()) {
        emit dataChanged(createIndex(m_selectedIndex, 0), createIndex(m_selectedIndex, 0));
    }
}

int LargeStepModel::smallStepCount(int largeStepIndex) const
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return 0;
    return m_steps.at(largeStepIndex).smallSteps.size();
}

SmallStepItem LargeStepModel::smallStepAt(int largeStepIndex, int smallStepIndex) const
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return SmallStepItem();
    const QList<SmallStepItem> &smallSteps = m_steps.at(largeStepIndex).smallSteps;
    if (smallStepIndex < 0 || smallStepIndex >= smallSteps.size())
        return SmallStepItem();
    return smallSteps.at(smallStepIndex);
}

void LargeStepModel::addSmallStep(int largeStepIndex, const QString &name)
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return;

    QList<SmallStepItem> &smallSteps = m_steps[largeStepIndex].smallSteps;
    SmallStepItem item;
    item.id = QUuid::createUuid().toString();
    item.name = name;
    item.order = smallSteps.size() + 1;
    smallSteps.append(item);

    emit smallStepsChanged(largeStepIndex);
}

void LargeStepModel::insertSmallStep(int largeStepIndex, int smallStepIndex, const QString &name)
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return;

    QList<SmallStepItem> &smallSteps = m_steps[largeStepIndex].smallSteps;
    if (smallStepIndex < 0 || smallStepIndex > smallSteps.size())
        smallStepIndex = smallSteps.size();

    SmallStepItem item;
    item.id = QUuid::createUuid().toString();
    item.name = name;
    item.order = smallStepIndex + 1;

    smallSteps.insert(smallStepIndex, item);
    for (int i = smallStepIndex + 1; i < smallSteps.size(); ++i) {
        smallSteps[i].order = i + 1;
    }

    emit smallStepsChanged(largeStepIndex);
}

void LargeStepModel::removeSmallStep(int largeStepIndex, int smallStepIndex)
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return;

    QList<SmallStepItem> &smallSteps = m_steps[largeStepIndex].smallSteps;
    if (smallStepIndex < 0 || smallStepIndex >= smallSteps.size())
        return;

    smallSteps.removeAt(smallStepIndex);
    for (int i = smallStepIndex; i < smallSteps.size(); ++i) {
        smallSteps[i].order = i + 1;
    }

    emit smallStepsChanged(largeStepIndex);
}

void LargeStepModel::moveSmallStep(int largeStepIndex, int from, int to)
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return;

    QList<SmallStepItem> &smallSteps = m_steps[largeStepIndex].smallSteps;
    if (from < 0 || from >= smallSteps.size() ||
        to < 0 || to >= smallSteps.size() ||
        from == to)
        return;

    smallSteps.move(from, to);
    for (int i = 0; i < smallSteps.size(); ++i) {
        smallSteps[i].order = i + 1;
    }

    emit smallStepsChanged(largeStepIndex);
}

void LargeStepModel::renameSmallStep(int largeStepIndex, int smallStepIndex, const QString &name)
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return;

    QList<SmallStepItem> &smallSteps = m_steps[largeStepIndex].smallSteps;
    if (smallStepIndex < 0 || smallStepIndex >= smallSteps.size())
        return;

    smallSteps[smallStepIndex].name = name;
    emit smallStepsChanged(largeStepIndex);
}

void LargeStepModel::setSmallStepResourceBinding(int largeStepIndex, int smallStepIndex, const QString &resource)
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return;

    QList<SmallStepItem> &smallSteps = m_steps[largeStepIndex].smallSteps;
    if (smallStepIndex < 0 || smallStepIndex >= smallSteps.size())
        return;

    smallSteps[smallStepIndex].resourceBinding = resource;
    emit smallStepsChanged(largeStepIndex);
}

void LargeStepModel::setSmallStepRuleBinding(int largeStepIndex, int smallStepIndex, const QString &rule)
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return;

    QList<SmallStepItem> &smallSteps = m_steps[largeStepIndex].smallSteps;
    if (smallStepIndex < 0 || smallStepIndex >= smallSteps.size())
        return;

    smallSteps[smallStepIndex].ruleBinding = rule;
    emit smallStepsChanged(largeStepIndex);
}

QString LargeStepModel::smallStepResourceBinding(int largeStepIndex, int smallStepIndex) const
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return QString();

    const QList<SmallStepItem> &smallSteps = m_steps.at(largeStepIndex).smallSteps;
    if (smallStepIndex < 0 || smallStepIndex >= smallSteps.size())
        return QString();

    return smallSteps.at(smallStepIndex).resourceBinding;
}

QString LargeStepModel::smallStepRuleBinding(int largeStepIndex, int smallStepIndex) const
{
    if (largeStepIndex < 0 || largeStepIndex >= m_steps.size())
        return QString();

    const QList<SmallStepItem> &smallSteps = m_steps.at(largeStepIndex).smallSteps;
    if (smallStepIndex < 0 || smallStepIndex >= smallSteps.size())
        return QString();

    return smallSteps.at(smallStepIndex).ruleBinding;
}

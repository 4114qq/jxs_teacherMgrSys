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
    int stepCount = m_steps.size();
    if (stepCount == 0)
        return 1;
    if (stepCount == 1)
        return 2;
    return stepCount * 2;
}

QVariant LargeStepModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int stepCount = m_steps.size();

    if (isAddItem(row)) {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return QString::fromLocal8Bit("点击添加步骤");
        if (role == Qt::UserRole + 2)
            return true;
        return QVariant();
    }

    if (isArrow(row)) {
        return QVariant();
    }

    int stepIndex;
    if (stepCount <= 1) {
        stepIndex = row;
    } else {
        stepIndex = row / 2;
    }

    if (stepIndex >= stepCount)
        return QVariant();

    const LargeStepItem &step = m_steps.at(stepIndex);

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
    int addItemRow = stepCount * 2;

    if (row == addItemRow) {
        if (role == Qt::EditRole) {
            QString name = value.toString();
            if (!name.isEmpty()) {
                addStep(name);
            }
            return true;
        }
        return false;
    }

    if (row > addItemRow)
        return false;

    bool isArrowItem = (row % 2 == 1);
    if (isArrowItem)
        return false;

    int stepIndex = row / 2;
    if (stepIndex >= stepCount)
        return false;

    if (role == Qt::EditRole) {
        m_steps[stepIndex].name = value.toString();
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

    if (isAddItem(row)) {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    if (isArrow(row)) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index) |
           Qt::ItemIsDragEnabled |
           Qt::ItemIsDropEnabled |
           Qt::ItemIsEditable;
}

bool LargeStepModel::isAddItem(int index) const
{
    int stepCount = m_steps.size();
    if (stepCount == 0)
        return index == 0;
    if (stepCount == 1)
        return index == 1;
    return index == stepCount * 2 - 1;
}

bool LargeStepModel::isArrow(int index) const
{
    int stepCount = m_steps.size();
    if (stepCount <= 1)
        return false;
    return index % 2 == 1 && index < stepCount * 2 - 2;
}

int LargeStepModel::addItemIndex() const
{
    int stepCount = m_steps.size();
    if (stepCount == 0)
        return 0;
    if (stepCount == 1)
        return 1;
    return stepCount * 2 - 1;
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
    if (stepCount <= 1)
        return false;

    if (isAddItem(sourceRow) || isAddItem(destinationChild))
        return false;

    if (destinationChild < 0)
        destinationChild = 0;
    else if (destinationChild >= rowCount())
        destinationChild = rowCount() - 1;

    if (isArrow(sourceRow))
        return false;

    int sourceStepIndex = sourceRow / 2;
    int destStepIndex = destinationChild / 2;

    if (sourceStepIndex >= stepCount || destStepIndex > stepCount)
        return false;

    if (sourceStepIndex == destStepIndex)
        return true;

    if (destStepIndex > sourceStepIndex)
        destStepIndex--;

    m_isMoving = true;
    beginResetModel();

    m_steps.move(sourceStepIndex, destStepIndex);

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
    int insertPos = m_steps.size();
    beginInsertRows(QModelIndex(), insertPos * 2, insertPos * 2);

    LargeStepItem item;
    item.id = QUuid::createUuid().toString();
    item.name = name;
    item.order = m_steps.size() + 1;
    item.isArrow = false;

    m_steps.append(item);
    endInsertRows();
    emit stepsChanged();
}

void LargeStepModel::insertStep(int index, const QString &name)
{
    if (index < 0 || index > m_steps.size())
        index = m_steps.size();

    beginInsertRows(QModelIndex(), index * 2, index * 2);

    LargeStepItem item;
    item.id = QUuid::createUuid().toString();
    item.name = name;
    item.order = index + 1;
    item.isArrow = false;

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

    beginRemoveRows(QModelIndex(), index * 2, index * 2);
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
    QModelIndex idx = createIndex(index * 2, 0);
    emit dataChanged(idx, idx);
    emit stepsChanged();
}

void LargeStepModel::clear()
{
    if (m_steps.isEmpty())
        return;

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
    int oldIndex = m_selectedIndex;
    m_selectedIndex = index;

    if (oldIndex >= 0 && oldIndex < m_steps.size()) {
        emit dataChanged(createIndex(oldIndex * 2, 0), createIndex(oldIndex * 2, 0));
    }
    if (m_selectedIndex >= 0 && m_selectedIndex < m_steps.size()) {
        emit dataChanged(createIndex(m_selectedIndex * 2, 0), createIndex(m_selectedIndex * 2, 0));
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
    item.isAddItem = false;

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
    item.isAddItem = false;

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
    if (from < 0 || from >= smallSteps.size() || to < 0 || to >= smallSteps.size() || from == to)
        return;

    beginResetModel();
    smallSteps.move(from, to);

    for (int i = 0; i < smallSteps.size(); ++i) {
        smallSteps[i].order = i + 1;
    }

    endResetModel();
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

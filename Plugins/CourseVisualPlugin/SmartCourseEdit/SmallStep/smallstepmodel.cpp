#include "smallstepmodel.h"
#include "../LargeStep/largestepmodel.h"

SmallStepModel::SmallStepModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_largeStepModel(nullptr)
    , m_largeStepIndex(-1)
    , m_selectedIndex(-1)
    , m_isMoving(false)
{
}

SmallStepModel::~SmallStepModel()
{
}

void SmallStepModel::setLargeStepModel(LargeStepModel *model)
{
    if (m_largeStepModel) {
        disconnect(m_largeStepModel, &LargeStepModel::smallStepsChanged,
                   this, &SmallStepModel::onLargeStepsChanged);
    }

    m_largeStepModel = model;

    if (m_largeStepModel) {
        connect(m_largeStepModel, &LargeStepModel::smallStepsChanged,
                this, &SmallStepModel::onLargeStepsChanged);
    }
}

void SmallStepModel::setLargeStepIndex(int index)
{
    if (m_largeStepIndex == index)
        return;

    m_largeStepIndex = index;
    m_selectedIndex = -1;
    beginResetModel();
    endResetModel();
}

void SmallStepModel::onLargeStepsChanged()
{
    if (m_largeStepIndex >= 0) {
        beginResetModel();
        endResetModel();
    }
}

int SmallStepModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_largeStepModel || m_largeStepIndex < 0)
        return 0;

    int count = m_largeStepModel->smallStepCount(m_largeStepIndex);
    return count + 1;
}

QVariant SmallStepModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_largeStepModel || m_largeStepIndex < 0)
        return QVariant();

    int row = index.row();
    int smallCount = m_largeStepModel->smallStepCount(m_largeStepIndex);

    if (row == smallCount) {
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return QString::fromLocal8Bit("点击添加步骤");
        if (role == Qt::UserRole + 2)
            return true;
        return QVariant();
    }

    if (row > smallCount)
        return QVariant();

    SmallStepItem item = m_largeStepModel->smallStepAt(m_largeStepIndex, row);

    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return item.name;
    case Qt::UserRole:
        return item.id;
    case Qt::UserRole + 1:
        return item.order;
    case Qt::UserRole + 2:
        return false;
    default:
        return QVariant();
    }
}

bool SmallStepModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || !m_largeStepModel || m_largeStepIndex < 0)
        return false;

    int row = index.row();
    int smallCount = m_largeStepModel->smallStepCount(m_largeStepIndex);

    if (row == smallCount) {
        if (role == Qt::EditRole) {
            QString name = value.toString();
            if (!name.isEmpty()) {
                m_largeStepModel->addSmallStep(m_largeStepIndex, name);
            }
            return true;
        }
        return false;
    }

    if (row > smallCount)
        return false;

    if (role == Qt::EditRole) {
        m_largeStepModel->renameSmallStep(m_largeStepIndex, row, value.toString());
        return true;
    }

    return false;
}

Qt::ItemFlags SmallStepModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsDropEnabled;

    int row = index.row();
    int smallCount = m_largeStepModel ? m_largeStepModel->smallStepCount(m_largeStepIndex) : 0;

    if (row >= smallCount) {
        return Qt::ItemIsEnabled | Qt::ItemIsEditable;
    }

    return QAbstractItemModel::flags(index) |
           Qt::ItemIsDragEnabled |
           Qt::ItemIsDropEnabled |
           Qt::ItemIsEditable;
}

bool SmallStepModel::isAddItem(int index) const
{
    if (!m_largeStepModel || m_largeStepIndex < 0)
        return false;
    int smallCount = m_largeStepModel->smallStepCount(m_largeStepIndex);
    return index == smallCount;
}

Qt::DropActions SmallStepModel::supportedDropActions() const
{
    return Qt::MoveAction;
}

bool SmallStepModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                              const QModelIndex &destinationParent, int destinationChild)
{
    if (sourceParent != destinationParent || count != 1 || m_isMoving)
        return false;

    int smallCount = m_largeStepModel ? m_largeStepModel->smallStepCount(m_largeStepIndex) : 0;

    if (sourceRow >= smallCount || destinationChild > smallCount)
        return false;

    if (sourceRow == destinationChild)
        return true;

    int destRow = destinationChild;
    if (destinationChild > sourceRow) {
        destRow = destinationChild - 1;
    }

    if (destRow >= smallCount)
        destRow = smallCount - 1;

    if (sourceRow == destRow)
        return true;

    m_isMoving = true;
    m_largeStepModel->moveSmallStep(m_largeStepIndex, sourceRow, destRow);
    m_isMoving = false;

    return true;
}

void SmallStepModel::setSelectedIndex(int index)
{
    if (index == m_selectedIndex)
        return;

    int oldIndex = m_selectedIndex;
    m_selectedIndex = index;

    if (oldIndex >= 0) {
        emit dataChanged(createIndex(oldIndex, 0), createIndex(oldIndex, 0));
    }
    if (m_selectedIndex >= 0) {
        emit dataChanged(createIndex(m_selectedIndex, 0), createIndex(m_selectedIndex, 0));
    }
}

QString SmallStepModel::resourceBinding(int index) const
{
    if (!m_largeStepModel || m_largeStepIndex < 0)
        return QString();
    return m_largeStepModel->smallStepResourceBinding(m_largeStepIndex, index);
}

QString SmallStepModel::ruleBinding(int index) const
{
    if (!m_largeStepModel || m_largeStepIndex < 0)
        return QString();
    return m_largeStepModel->smallStepRuleBinding(m_largeStepIndex, index);
}

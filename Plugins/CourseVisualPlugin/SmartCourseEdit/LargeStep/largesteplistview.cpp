#include "largesteplistview.h"
#include "largestepmodel.h"
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

LargeStepListView::LargeStepListView(QWidget *parent)
    : QListView(parent)
    , m_model(nullptr)
{
    setFlow(QListView::LeftToRight);
    setSpacing(10);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setMaximumHeight(100);
    setDragDropMode(QAbstractItemView::InternalMove);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
}

void LargeStepListView::setStepModel(LargeStepModel *model)
{
    m_model = model;
    setModel(model);

    if (model) {
        connect(selectionModel(), &QItemSelectionModel::currentChanged,
                this, [this](const QModelIndex &current, const QModelIndex &previous) {
            Q_UNUSED(previous)
            if (current.isValid() && !m_model->isAddItem(current.row()) && !m_model->isArrow(current.row())) {
                emit stepSelectionChanged(current.row() / 2);
            }
        });
    }
}

void LargeStepListView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
        return;

    int row = index.row();
    if (m_model->isAddItem(row)) {
        emit addItemPressed(row);
        return;
    }

    if (m_model->isArrow(row))
        return;

    QListView::mousePressEvent(event);
}

void LargeStepListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
        return;

    int row = index.row();
    if (m_model->isAddItem(row) || m_model->isArrow(row))
        return;

    QListView::mouseDoubleClickEvent(event);
}

void LargeStepListView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
        return;

    int row = index.row();
    if (m_model->isAddItem(row) || m_model->isArrow(row))
        return;

    QListView::contextMenuEvent(event);
}

QModelIndex LargeStepListView::moveCursor(CursorAction cursor, Qt::KeyboardModifiers modifiers)
{
    QModelIndex current = currentIndex();
    if (!current.isValid() || !m_model)
        return QListView::moveCursor(cursor, modifiers);

    int currentRow = current.row();
    int newRow = currentRow;

    if (cursor == MoveLeft) {
        newRow = currentRow - 1;
        while (newRow >= 0 && m_model->isArrow(newRow)) {
            newRow--;
        }
        if (newRow < 0)
            newRow = 0;
    } else if (cursor == MoveRight) {
        newRow = currentRow + 1;
        while (newRow < m_model->rowCount() && m_model->isArrow(newRow)) {
            newRow++;
        }
        if (newRow >= m_model->rowCount())
            newRow = m_model->rowCount() - 1;
    }

    return m_model->index(newRow, 0);
}

int LargeStepListView::findTargetStepIndex(const QPoint &pos)
{
    QModelIndex index = indexAt(pos);

    if (!index.isValid()) {
        int targetIndex = -1;
        for (int i = 0; i < m_model->rowCount(); ++i) {
            QRect rect = visualRect(m_model->index(i, 0));
            if (pos.x() < rect.right() + spacing() / 2) {
                targetIndex = i;
                break;
            }
        }
        if (targetIndex < 0) {
            targetIndex = m_model->rowCount() - 1;
        }

        if (m_model->isArrow(targetIndex) || m_model->isAddItem(targetIndex)) {
            int prevRow = targetIndex - 1;
            while (prevRow >= 0 && (m_model->isArrow(prevRow) || m_model->isAddItem(prevRow))) {
                prevRow--;
            }
            if (prevRow >= 0) {
                return prevRow / 2 + 1;
            }
            return -1;
        }

        int targetStepIndex = targetIndex / 2;
        QRect rect = visualRect(m_model->index(targetIndex, 0));
        if (pos.x() > rect.right() - rect.width() / 2) {
            return targetStepIndex + 1;
        }
        return targetStepIndex;
    }

    int row = index.row();

    if (m_model->isArrow(row)) {
        QRect rect = visualRect(index);
        int prevRow = row - 1;
        while (prevRow >= 0 && (m_model->isArrow(prevRow) || m_model->isAddItem(prevRow))) {
            prevRow--;
        }
        int nextRow = row + 1;
        while (nextRow < m_model->rowCount() && (m_model->isArrow(nextRow) || m_model->isAddItem(nextRow))) {
            nextRow++;
        }

        if (pos.x() < rect.center().x()) {
            if (nextRow < m_model->rowCount()) {
                return nextRow / 2;
            }
        } else {
            if (prevRow >= 0) {
                return prevRow / 2 + 1;
            }
        }
        return -1;
    }

    if (m_model->isAddItem(row)) {
        int prevRow = row - 1;
        while (prevRow >= 0 && (m_model->isArrow(prevRow) || m_model->isAddItem(prevRow))) {
            prevRow--;
        }
        if (prevRow >= 0) {
            return prevRow / 2 + 1;
        }
        return -1;
    }

    QRect rect = visualRect(index);
    int stepIndex = row / 2;

    if (pos.x() < rect.center().x()) {
        return stepIndex;
    } else {
        return stepIndex + 1;
    }
}

void LargeStepListView::dragMoveEvent(QDragMoveEvent *event)
{
    int targetIndex = findTargetStepIndex(event->pos());
    if (targetIndex >= 0) {
        event->accept();
        return;
    }
    event->ignore();
}

void LargeStepListView::dropEvent(QDropEvent *event)
{
    int targetStepIndex = findTargetStepIndex(event->pos());
    if (targetStepIndex < 0) {
        event->ignore();
        return;
    }

    QModelIndexList selected = selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        event->ignore();
        return;
    }

    int sourceRow = selected.first().row();
    int sourceStepIndex = sourceRow / 2;

    if (sourceStepIndex == targetStepIndex) {
        event->accept();
        return;
    }

    int destStepIndex = targetStepIndex;
    if (destStepIndex > sourceStepIndex) {
        destStepIndex--;
    }

    if (destStepIndex < 0) destStepIndex = 0;

    int maxStepIndex = m_model->rowCount() / 2 - 1;
    if (destStepIndex > maxStepIndex) destStepIndex = maxStepIndex;

    if (sourceStepIndex != destStepIndex) {
        m_model->moveStep(sourceStepIndex, destStepIndex);

        int newRow = destStepIndex * 2;
        if (newRow >= m_model->rowCount())
            newRow = m_model->rowCount() - 1;
        QModelIndex newIndex = m_model->index(newRow, 0);
        setCurrentIndex(newIndex);
    }

    event->accept();
}
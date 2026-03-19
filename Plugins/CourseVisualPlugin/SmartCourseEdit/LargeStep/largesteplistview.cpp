#include "largesteplistview.h"
#include "largestepmodel.h"
#include <QMouseEvent>
#include <QContextMenuEvent>

LargeStepListView::LargeStepListView(QWidget *parent)
    : QListView(parent)
    , m_model(nullptr)
{
    setFlow(QListView::LeftToRight);
    setSpacing(10);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void LargeStepListView::setStepModel(LargeStepModel *model)
{
    m_model = model;
    setModel(model);

    if (model) {
        connect(selectionModel(), &QItemSelectionModel::currentChanged,
                this, [this](const QModelIndex &current, const QModelIndex &previous) {
            Q_UNUSED(previous)
            if (current.isValid() && !m_model->isAddItem(current.row())) {
                emit stepSelectionChanged(current.row());
            }
        });
    }
}

void LargeStepListView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && m_model && m_model->isAddItem(index.row())) {
        emit addItemPressed(index.row());
        return;
    }
    QListView::mousePressEvent(event);
}

void LargeStepListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && m_model && m_model->isAddItem(index.row())) {
        emit addItemClicked(index);
        return;
    }
    QListView::mouseDoubleClickEvent(event);
}

void LargeStepListView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && m_model && m_model->isAddItem(index.row())) {
        return;
    }
    QListView::contextMenuEvent(event);
}

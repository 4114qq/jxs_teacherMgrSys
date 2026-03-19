#include "smallsteplistview.h"
#include "smallstepmodel.h"
#include <QMouseEvent>
#include <QContextMenuEvent>

SmallStepListView::SmallStepListView(QWidget *parent)
    : QListView(parent)
    , m_model(nullptr)
{
    setFlow(QListView::TopToBottom);
    setSpacing(8);
    setContextMenuPolicy(Qt::CustomContextMenu);
}

void SmallStepListView::setStepModel(SmallStepModel *model)
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

void SmallStepListView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && m_model && m_model->isAddItem(index.row())) {
        emit addItemPressed(index.row());
        return;
    }
    QListView::mousePressEvent(event);
}

void SmallStepListView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && m_model && m_model->isAddItem(index.row())) {
        emit addItemClicked(index);
        return;
    }
    QListView::mouseDoubleClickEvent(event);
}

void SmallStepListView::contextMenuEvent(QContextMenuEvent *event)
{
    QModelIndex index = indexAt(event->pos());
    if (index.isValid() && m_model && m_model->isAddItem(index.row())) {
        return;
    }
    QListView::contextMenuEvent(event);
}

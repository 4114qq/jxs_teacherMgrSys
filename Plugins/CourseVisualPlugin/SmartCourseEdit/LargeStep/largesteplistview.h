#ifndef LARGESTEPLISTVIEW_H
#define LARGESTEPLISTVIEW_H

#include <QListView>

class LargeStepModel;

class LargeStepListView : public QListView
{
    Q_OBJECT

public:
    explicit LargeStepListView(QWidget *parent = nullptr);

    void setStepModel(LargeStepModel *model);
    LargeStepModel* stepModel() const { return m_model; }

signals:
    void addItemClicked(const QModelIndex &index);
    void addItemPressed(int index);
    void stepSelectionChanged(int index);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    QModelIndex moveCursor(CursorAction cursor, Qt::KeyboardModifiers modifiers) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    LargeStepModel *m_model;
    int findTargetStepIndex(const QPoint &pos);
};

#endif
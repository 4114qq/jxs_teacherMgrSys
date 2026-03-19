#ifndef SMALLSTEPLISTVIEW_H
#define SMALLSTEPLISTVIEW_H

#include <QListView>

class SmallStepModel;

class SmallStepListView : public QListView
{
    Q_OBJECT

public:
    explicit SmallStepListView(QWidget *parent = nullptr);

    void setStepModel(SmallStepModel *model);
    SmallStepModel* stepModel() const { return m_model; }

signals:
    void addItemClicked(const QModelIndex &index);
    void addItemPressed(int index);
    void stepSelectionChanged(int index);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private:
    SmallStepModel *m_model;
};

#endif

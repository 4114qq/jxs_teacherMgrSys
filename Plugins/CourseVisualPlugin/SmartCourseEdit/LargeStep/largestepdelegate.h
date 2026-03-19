#ifndef LARGESTEPDELEGATE_H
#define LARGESTEPDELEGATE_H

#include <QStyledItemDelegate>

class LargeStepModel;

class LargeStepDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit LargeStepDelegate(QObject *parent = nullptr);
    ~LargeStepDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setModel(LargeStepModel *model);
    bool isEditing() const { return m_isEditing; }
    void setEditingIndex(const QModelIndex &index);
    bool isIndexEditing(const QModelIndex &index) const;

private:
    void drawStepItem(QPainter *painter, const QRect &rect, const QString &name, bool isSelected) const;
    void drawArrow(QPainter *painter, const QRect &rect) const;
    void drawAddItem(QPainter *painter, const QRect &rect) const;

    mutable LargeStepModel *m_model;
    mutable bool m_isEditing;
    mutable QPersistentModelIndex m_editingIndex;
};

#endif

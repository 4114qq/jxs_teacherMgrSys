#ifndef SMALLSTEPDELEGATE_H
#define SMALLSTEPDELEGATE_H

#include <QStyledItemDelegate>

class SmallStepModel;

class SmallStepDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit SmallStepDelegate(QObject *parent = nullptr);
    ~SmallStepDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setModel(SmallStepModel *model);
    bool isEditing() const { return m_isEditing; }
    void setEditingIndex(const QModelIndex &index);
    bool isIndexEditing(const QModelIndex &index) const;

private:
    void drawStepItem(QPainter *painter, const QRect &rect, const QString &name, bool isSelected) const;
    void drawAddItem(QPainter *painter, const QRect &rect) const;

    mutable SmallStepModel *m_model;
    mutable bool m_isEditing;
    mutable QPersistentModelIndex m_editingIndex;
};

#endif

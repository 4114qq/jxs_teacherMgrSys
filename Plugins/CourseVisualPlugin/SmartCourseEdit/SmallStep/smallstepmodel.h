#ifndef SMALLSTEPMODEL_H
#define SMALLSTEPMODEL_H

#include <QAbstractListModel>
#include "../LargeStep/largestepitem.h"

class LargeStepModel;

class SmallStepModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SmallStepModel(QObject *parent = nullptr);
    ~SmallStepModel();

    void setLargeStepModel(LargeStepModel *model);
    void setLargeStepIndex(int index);
    int largeStepIndex() const { return m_largeStepIndex; }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                  const QModelIndex &destinationParent, int destinationChild) override;

    bool isAddItem(int index) const;
    int selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(int index);

    QString resourceBinding(int index) const;
    QString ruleBinding(int index) const;

signals:
    void smallStepsChanged();

private slots:
    void onLargeStepsChanged();

private:
    LargeStepModel *m_largeStepModel;
    int m_largeStepIndex;
    int m_selectedIndex;
    bool m_isMoving;
};

#endif

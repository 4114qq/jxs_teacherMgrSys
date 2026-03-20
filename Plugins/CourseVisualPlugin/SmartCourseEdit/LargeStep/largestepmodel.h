#ifndef LARGESTEPMODEL_H
#define LARGESTEPMODEL_H

#include <QAbstractListModel>
#include <QList>
#include "largestepitem.h"

class LargeStepModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit LargeStepModel(QObject *parent = nullptr);
    ~LargeStepModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    Qt::DropActions supportedDropActions() const override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count,
                  const QModelIndex &destinationParent, int destinationChild) override;

    void addStep(const QString &name);
    void insertStep(int index, const QString &name);
    void removeStep(int index);
    void moveStep(int from, int to);
    void renameStep(int index, const QString &name);
    void clear();
    LargeStepItem stepAt(int index) const;
    bool isEmpty() const;
    bool isAddItem(int index) const;
    bool isArrow(int index) const;
    int addItemIndex() const;

    int selectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(int index);

    int smallStepCount(int largeStepIndex) const;
    SmallStepItem smallStepAt(int largeStepIndex, int smallStepIndex) const;
    void addSmallStep(int largeStepIndex, const QString &name);
    void insertSmallStep(int largeStepIndex, int smallStepIndex, const QString &name);
    void removeSmallStep(int largeStepIndex, int smallStepIndex);
    void moveSmallStep(int largeStepIndex, int from, int to);
    void renameSmallStep(int largeStepIndex, int smallStepIndex, const QString &name);
    void setSmallStepResourceBinding(int largeStepIndex, int smallStepIndex, const QString &resource);
    void setSmallStepRuleBinding(int largeStepIndex, int smallStepIndex, const QString &rule);
    QString smallStepResourceBinding(int largeStepIndex, int smallStepIndex) const;
    QString smallStepRuleBinding(int largeStepIndex, int smallStepIndex) const;

signals:
    void stepsChanged();
    void smallStepsChanged(int largeStepIndex);

private:
    QList<LargeStepItem> m_steps;
    int m_selectedIndex;
    bool m_isMoving;
};

#endif

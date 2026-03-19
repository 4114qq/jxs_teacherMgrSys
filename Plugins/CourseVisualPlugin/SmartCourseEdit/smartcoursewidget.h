#ifndef SMARTCOURSEWIDGET_H
#define SMARTCOURSEWIDGET_H

#include <QWidget>
#include <QMenu>
#include "LargeStep/largestepmodel.h"
#include "LargeStep/largestepdelegate.h"
#include "LargeStep/largesteplistview.h"
#include "SmallStep/smallstepmodel.h"
#include "SmallStep/smallstepdelegate.h"
#include "SmallStep/smallsteplistview.h"

namespace Ui {
class SmartCourseWidget;
}

class SmartCourseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SmartCourseWidget(QWidget *parent = nullptr);
    ~SmartCourseWidget();

    QStringList getLargeSteps() const;
    void setLargeSteps(const QStringList &steps);
    void clearAll();

signals:
    void largeStepsChanged();

private slots:
    void onLargeContextMenuRequested(const QPoint &pos);
    void onSmallContextMenuRequested(const QPoint &pos);
    void onInsertLargeStepBefore();
    void onInsertLargeStepAfter();
    void onMoveLargeStepUp();
    void onMoveLargeStepDown();
    void onRenameLargeStep();
    void onDeleteLargeStep();
    void onClearAllLargeSteps();
    void onLargeStepSelectionChanged(int index);
    void onLargeStepsChanged();

    void onInsertSmallStepBefore();
    void onInsertSmallStepAfter();
    void onMoveSmallStepUp();
    void onMoveSmallStepDown();
    void onRenameSmallStep();
    void onDeleteSmallStep();
    void onClearAllSmallSteps();
    void onResourceBinding();
    void onRuleBinding();

    void onAddLargeItemPressed(int index);
    void onAddLargeItemClicked(const QModelIndex &index);
    void onAddSmallItemPressed(int index);
    void onAddSmallItemClicked(const QModelIndex &index);
    void onSmallStepSelectionChanged(int index);

private:
    Ui::SmartCourseWidget *ui;
    LargeStepModel *m_largeStepModel;
    LargeStepDelegate *m_largeStepDelegate;
    SmallStepModel *m_smallStepModel;
    SmallStepDelegate *m_smallStepDelegate;
    QMenu *m_largeItemMenu;
    QMenu *m_largeEmptyMenu;
    QMenu *m_smallItemMenu;
    QMenu *m_smallEmptyMenu;

    void setupLargeContextMenus();
    void setupLargeEmptyMenu();
    void setupSmallContextMenus();
    void setupSmallEmptyMenu();
};

#endif

#ifndef LARGESTEPITEM_H
#define LARGESTEPITEM_H

#include <QString>
#include <QList>

struct SmallStepItem {
    QString id;
    QString name;
    int order;
    bool isAddItem;
    QString resourceBinding;
    QString ruleBinding;
};

struct LargeStepItem {
    QString id;
    QString name;
    int order;
    bool isAddItem;
    bool isArrow;
    QList<SmallStepItem> smallSteps;
};

#endif

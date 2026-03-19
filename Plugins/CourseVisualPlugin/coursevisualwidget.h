#ifndef COURSEVISUALWIDGET_H
#define COURSEVISUALWIDGET_H

#include <QWidget>
#include "SmartCourseEdit/smartcoursewidget.h"

namespace Ui {
class CourseVisualWidget;
}

class CourseVisualWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CourseVisualWidget(QWidget *parent = nullptr);
    ~CourseVisualWidget();

private:
    Ui::CourseVisualWidget *ui;
    SmartCourseWidget *m_smartCourseWidget;
};

#endif // COURSEVISUALWIDGET_H
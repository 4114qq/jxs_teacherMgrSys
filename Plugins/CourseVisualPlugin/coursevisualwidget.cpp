#include "coursevisualwidget.h"
#include "ui_coursevisualwidget.h"
#include <QVBoxLayout>

CourseVisualWidget::CourseVisualWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CourseVisualWidget)
    , m_smartCourseWidget(new SmartCourseWidget(this))
{
    ui->setupUi(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_smartCourseWidget);
    setLayout(layout);
}

CourseVisualWidget::~CourseVisualWidget()
{
    delete ui;
}

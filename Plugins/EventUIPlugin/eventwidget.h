#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include <QWidget>
#include "../../common/interfaces/IBaseEventBus.h"
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QTimer>

namespace Ui {
class EventWidget;
}

class EventWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EventWidget(QWidget *parent = nullptr);
    ~EventWidget();

    void setEventManager(IBaseEventBus *manager);

private slots:
    void onEventPublished(const QString &eventType, const QVariantMap &data);
    void onClearEvents();
    void onPublishEvent();
    void onAutoScrollToggled(bool checked);

private:
    Ui::EventWidget *ui;
    IBaseEventBus *m_eventManager;
    QTimer *m_refreshTimer;
    void refreshEvents();
};

#endif // EVENTWIDGET_H

#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include "../../common/interfaces/ILogManager.h"
#include <QSqlQuery>
#include <QCheckBox>
#include <QTimer>

namespace Ui {
class LogWidget;
}

class LogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = nullptr);
    ~LogWidget();

    void setLogManager(ILogManager *manager);

private slots:
    void onClearLogs();
    void onExportLogs();
    void onLevelFilterChanged();
    void onRefreshLogs();

private:
    Ui::LogWidget *ui;
    ILogManager *m_logManager;
    QTimer *m_refreshTimer;
    void refreshLogs();
    QString levelToString(LogLevel level) const;
    QString getLevelColor(LogLevel level) const;
};

#endif // LOGWIDGET_H

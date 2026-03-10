#ifndef DATABASEWIDGET_H
#define DATABASEWIDGET_H

#include <QWidget>
#include <QTimer>
#include "../../common/interfaces/IDatabaseManager.h"
#include <QSqlQuery>

namespace Ui {
class DatabaseWidget;
}

class DatabaseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DatabaseWidget(QWidget *parent = nullptr);
    ~DatabaseWidget();

    void setDatabaseManager(IDatabaseManager *manager);

private slots:
    void onExecuteQuery();
    void onUpdateStatus();

private:
    Ui::DatabaseWidget *ui;
    IDatabaseManager *m_databaseManager;
    QTimer *m_statusTimer;
    void updateConnectionStatus();
};

#endif // DATABASEWIDGET_H

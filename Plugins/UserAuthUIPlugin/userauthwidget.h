#ifndef USERAUTHWIDGET_H
#define USERAUTHWIDGET_H

#include <QWidget>
#include "../../common/interfaces/IAuthManager.h"
#include "../../common/interfaces/ILogManager.h"

namespace Ui {
class UserAuthWidget;
}

class UserAuthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UserAuthWidget(QWidget *parent = nullptr);
    ~UserAuthWidget();

    void setAuthManager(IAuthManager *manager);
    void setLogManager(ILogManager *manager);

private slots:
    void onBtnLoginClicked();
    void onBtnLogoutClicked();
    void onBtnRefreshClicked();
    void onBtnUpdateRoleClicked();
    void onBtnFingerprintClicked();
    void onBtnFaceClicked();

private:
    Ui::UserAuthWidget *ui;
    IAuthManager *m_authManager;
    ILogManager *m_logManager;
    int m_currentUserId;

    void loadUsers();
    void appendResult(const QString &text);
    void setLoggedIn(bool loggedIn);
};

#endif // USERAUTHWIDGET_H

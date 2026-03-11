#include "userauthwidget.h"
#include "ui_userauthwidget.h"
#include <QMessageBox>
#include <QDateTime>
#include <QTextCursor>
#include <QTableWidgetItem>

UserAuthWidget::UserAuthWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UserAuthWidget)
    , m_authManager(nullptr)
    , m_logManager(nullptr)
    , m_currentUserId(-1)
{
    ui->setupUi(this);

    connect(ui->btnLogin, &QPushButton::clicked, this, &UserAuthWidget::onBtnLoginClicked);
    connect(ui->btnLogout, &QPushButton::clicked, this, &UserAuthWidget::onBtnLogoutClicked);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &UserAuthWidget::onBtnRefreshClicked);
    connect(ui->btnUpdateRole, &QPushButton::clicked, this, &UserAuthWidget::onBtnUpdateRoleClicked);
    connect(ui->btnFingerprint, &QPushButton::clicked, this, &UserAuthWidget::onBtnFingerprintClicked);
    connect(ui->btnFace, &QPushButton::clicked, this, &UserAuthWidget::onBtnFaceClicked);

    loadUsers();
}

UserAuthWidget::~UserAuthWidget()
{
    delete ui;
}

void UserAuthWidget::setAuthManager(IAuthManager *manager)
{
    m_authManager = manager;
}

void UserAuthWidget::setLogManager(ILogManager *manager)
{
    m_logManager = manager;
}

void UserAuthWidget::onBtnLoginClicked()
{
    QString account = ui->lineEditAccount->text().trimmed();
    QString password = ui->lineEditPassword->text();

    if (account.isEmpty() || password.isEmpty()) {
        appendResult(QString::fromLocal8Bit("[错误] 账号和密码不能为空"));
        if (m_logManager) {
            m_logManager->logError("UserAuth", "Login failed: empty account or password");
        }
        return;
    }

    if (!m_authManager) {
        appendResult(QString::fromLocal8Bit("[错误] AuthManager 未初始化"));
        if (m_logManager) {
            m_logManager->logError("UserAuth", "AuthManager not initialized");
        }
        return;
    }

    bool success = m_authManager->login(account, password);

    if (success) {
        QVariantMap user = m_authManager->getCurrentUser();
        m_currentUserId = user.value("id").toInt();
        QString userName = user.value("Name").toString();
        QString loginType = (m_authManager->getCurrentLoginType() == IAuthManager::LoginType_Password) ? QString::fromLocal8Bit("密码") : QString::fromLocal8Bit("未知");

        QString result = QString::fromLocal8Bit("[成功] 登录成功！\n  账号: %1\n  姓名: %2\n  登录方式: %3\n  时间: %4")
            .arg(account).arg(userName).arg(loginType).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

        appendResult(result);
        setLoggedIn(true);

        if (m_logManager) {
            m_logManager->logInfo("UserAuth", QString::fromLocal8Bit("User logged in: %1 (%2)").arg(account).arg(userName));
        }
    } else {
        QString result = QString::fromLocal8Bit("[失败] 登录失败！\n  账号: %1\n  时间: %2")
            .arg(account).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

        appendResult(result);

        if (m_logManager) {
            m_logManager->logWarning("UserAuth", QString::fromLocal8Bit("Login failed for account: %1").arg(account));
        }
    }
}

void UserAuthWidget::onBtnLogoutClicked()
{
    if (m_authManager) {
        QString userName = m_authManager->getCurrentUser().value("Name").toString();
        m_authManager->logout();

        QString result = QString::fromLocal8Bit("[信息] 已退出登录\n  时间: %1")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

        appendResult(result);

        if (m_logManager) {
            m_logManager->logInfo("UserAuth", QString::fromLocal8Bit("User logged out: %1").arg(userName));
        }
    }

    m_currentUserId = -1;
    setLoggedIn(false);
    ui->lineEditAccount->clear();
    ui->lineEditPassword->clear();
}

void UserAuthWidget::onBtnFingerprintClicked()
{
    appendResult(QString::fromLocal8Bit("[信息] 指纹验证功能待实现（等待SDK）"));

    if (m_logManager) {
        m_logManager->logInfo("UserAuth", "Fingerprint verification called (not implemented)");
    }
}

void UserAuthWidget::onBtnFaceClicked()
{
    appendResult(QString::fromLocal8Bit("[信息] 人脸验证功能待实现（等待SDK）"));

    if (m_logManager) {
        m_logManager->logInfo("UserAuth", "Face verification called (not implemented)");
    }
}

void UserAuthWidget::onBtnRefreshClicked()
{
    loadUsers();
    appendResult(QString::fromLocal8Bit("[信息] 用户列表已刷新"));

    if (m_logManager) {
        m_logManager->logInfo("UserAuth", "User list refreshed");
    }
}

void UserAuthWidget::onBtnUpdateRoleClicked()
{
    int currentRow = ui->tableWidgetUsers->currentRow();
    if (currentRow < 0) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请先选择要修改的用户"));
        return;
    }

    QString newRole = ui->lineEditNewRole->text().trimmed();
    if (newRole.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请输入新角色ID"));
        return;
    }

    int userId = ui->tableWidgetUsers->item(currentRow, 0)->text().toInt();

    if (!m_authManager) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("AuthManager 未初始化"));
        return;
    }

    QStringList roleIds;
    QStringList parts = newRole.split(",", Qt::SkipEmptyParts);
    for (const QString &part : parts) {
        roleIds.append(part.trimmed());
    }

    QList<int> roleIdsInt;
    for (const QString &id : roleIds) {
        roleIdsInt.append(id.toInt());
    }

    bool success = m_authManager->assignRolesToUser(userId, roleIdsInt);

    if (success) {
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("角色修改成功"));

        QString result = QString::fromLocal8Bit("[成功] 角色修改成功\n  用户ID: %1\n  新角色ID: %2\n  时间: %3")
            .arg(userId).arg(newRole).arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

        appendResult(result);

        if (m_logManager) {
            m_logManager->logInfo("UserAuth", QString::fromLocal8Bit("Role updated for user ID %1: %2").arg(userId).arg(newRole));
        }

        loadUsers();
    } else {
        QMessageBox::critical(this, QString::fromLocal8Bit("失败"), QString::fromLocal8Bit("角色修改失败"));

        if (m_logManager) {
            m_logManager->logError("UserAuth", QString::fromLocal8Bit("Failed to update role for user ID %1").arg(userId));
        }
    }
}

void UserAuthWidget::loadUsers()
{
    if (!m_authManager) {
        return;
    }

    QVariantList users = m_authManager->getUsers();

    ui->tableWidgetUsers->setRowCount(users.size());

    for (int i = 0; i < users.size(); ++i) {
        const QVariantMap &user = users.at(i).toMap();

        ui->tableWidgetUsers->setItem(i, 0, new QTableWidgetItem(QString::number(user.value("id").toInt())));
        ui->tableWidgetUsers->setItem(i, 1, new QTableWidgetItem(user.value("PersonNum").toString()));
        ui->tableWidgetUsers->setItem(i, 2, new QTableWidgetItem(user.value("Name").toString()));
        ui->tableWidgetUsers->setItem(i, 3, new QTableWidgetItem(user.value("RoleIDS").toString()));
    }
}

void UserAuthWidget::appendResult(const QString &text)
{
    QString currentText = ui->textEditResult->toPlainText();
    if (!currentText.isEmpty()) {
        currentText += "\n";
    }
    currentText += text;
    ui->textEditResult->setPlainText(currentText);
    ui->textEditResult->moveCursor(QTextCursor::End);
}

void UserAuthWidget::setLoggedIn(bool loggedIn)
{
    ui->btnLogin->setEnabled(!loggedIn);
    ui->btnFingerprint->setEnabled(!loggedIn);
    ui->btnFace->setEnabled(!loggedIn);
    ui->btnLogout->setEnabled(loggedIn);
    ui->lineEditAccount->setEnabled(!loggedIn);
    ui->lineEditPassword->setEnabled(!loggedIn);
}

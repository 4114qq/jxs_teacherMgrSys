#include "databasewidget.h"
#include "ui_databasewidget.h"
#include <QMessageBox>
#include <QSqlRecord>
#include <QPushButton>
#include <QHeaderView>

DatabaseWidget::DatabaseWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DatabaseWidget)
    , m_databaseManager(nullptr)
    , m_statusTimer(new QTimer(this))
{
    ui->setupUi(this);

    connect(ui->btnExecute, &QPushButton::clicked, this, &DatabaseWidget::onExecuteQuery);
    connect(m_statusTimer, &QTimer::timeout, this, &DatabaseWidget::onUpdateStatus);
    m_statusTimer->start(3000);
    updateConnectionStatus();
}

DatabaseWidget::~DatabaseWidget()
{
    delete ui;
}

void DatabaseWidget::setDatabaseManager(IDatabaseManager *manager)
{
    m_databaseManager = manager;
    updateConnectionStatus();
}

void DatabaseWidget::updateConnectionStatus()
{
    if (!m_databaseManager) {
        ui->labelStatusValue->setText(QString::fromLocal8Bit("未连接"));
        ui->labelStatusValue->setStyleSheet(QString::fromLocal8Bit("color: red;"));
        ui->labelTypeValue->setText("-");
        ui->labelHostValue->setText("-");
        ui->labelPortValue->setText("-");
        ui->labelDatabaseValue->setText("-");
        ui->labelErrorValue->setText("-");
        return;
    }

    bool connected = m_databaseManager->isConnected();
    if (connected) {
        ui->labelStatusValue->setText(QString::fromLocal8Bit("已连接"));
        ui->labelStatusValue->setStyleSheet(QString::fromLocal8Bit("color: green;"));
    } else {
        ui->labelStatusValue->setText(QString::fromLocal8Bit("未连接"));
        ui->labelStatusValue->setStyleSheet(QString::fromLocal8Bit("color: red;"));
    }

    ui->labelTypeValue->setText(m_databaseManager->databaseType());
    ui->labelErrorValue->setText(m_databaseManager->lastError());
}

void DatabaseWidget::onExecuteQuery()
{
    if (!m_databaseManager) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("数据库管理器未初始化"));
        return;
    }

    if (!m_databaseManager->isConnected()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("数据库未连接"));
        return;
    }

    QString sql = ui->textEditQuery->toPlainText().trimmed();
    if (sql.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请输入 SQL 查询语句"));
        return;
    }

    QSharedPointer<QSqlQuery> query = m_databaseManager->query(sql);
    if (!query) {
        QString error = m_databaseManager->lastError();
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("查询失败: %1").arg(error));
        return;
    }

    ui->tableWidgetResult->clear();
    ui->tableWidgetResult->setRowCount(0);
    ui->tableWidgetResult->setColumnCount(0);

    if (query->isSelect()) {
        QSqlRecord record = query->record();
        int columnCount = record.count();
        if (columnCount == 0) {
            ui->labelRowCount->setText(QString::fromLocal8Bit("执行成功，无返回结果"));
            return;
        }

        ui->tableWidgetResult->setColumnCount(columnCount);
        for (int i = 0; i < columnCount; ++i) {
            ui->tableWidgetResult->setHorizontalHeaderItem(i, new QTableWidgetItem(record.fieldName(i)));
        }

        int row = 0;
        while (query->next()) {
            ui->tableWidgetResult->insertRow(row);
            for (int col = 0; col < columnCount; ++col) {
                QTableWidgetItem *item = new QTableWidgetItem(query->value(col).toString());
                ui->tableWidgetResult->setItem(row, col, item);
            }
            ++row;
        }

        ui->tableWidgetResult->resizeColumnsToContents();
        ui->labelRowCount->setText(QString::fromLocal8Bit("共 %1 行").arg(row));
    } else {
        ui->labelRowCount->setText(QString::fromLocal8Bit("执行成功，受影响行数: %1").arg(query->numRowsAffected()));
    }
}

void DatabaseWidget::onUpdateStatus()
{
    updateConnectionStatus();
}

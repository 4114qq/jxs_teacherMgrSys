#include "logwidget.h"
#include "ui_logwidget.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include <QPushButton>
#include <QAbstractItemView>

LogWidget::LogWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogWidget)
    , m_logManager(nullptr)
    , m_refreshTimer(new QTimer(this))
{
    ui->setupUi(this);

    connect(ui->btnClear, &QPushButton::clicked, this, &LogWidget::onClearLogs);
    connect(ui->btnExport, &QPushButton::clicked, this, &LogWidget::onExportLogs);
    connect(ui->checkDebug, &QCheckBox::stateChanged, this, &LogWidget::onLevelFilterChanged);
    connect(ui->checkInfo, &QCheckBox::stateChanged, this, &LogWidget::onLevelFilterChanged);
    connect(ui->checkWarning, &QCheckBox::stateChanged, this, &LogWidget::onLevelFilterChanged);
    connect(ui->checkError, &QCheckBox::stateChanged, this, &LogWidget::onLevelFilterChanged);
    connect(m_refreshTimer, &QTimer::timeout, this, &LogWidget::onRefreshLogs);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("时间") << QString::fromLocal8Bit("级别") << QString::fromLocal8Bit("标签") << QString::fromLocal8Bit("内容"));
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    m_refreshTimer->start(1000);
}

LogWidget::~LogWidget()
{
    m_refreshTimer->stop();
    delete ui;
}

void LogWidget::setLogManager(ILogManager *manager)
{
    m_logManager = manager;
    if (m_logManager) {
        refreshLogs();
    }
}

void LogWidget::onClearLogs()
{
    if (m_logManager) {
        m_logManager->clearLogs();
        ui->tableWidget->setRowCount(0);
    }
}

void LogWidget::onExportLogs()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        QString::fromLocal8Bit("导出日志"),
        QString("log_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "Text Files (*.txt);;All Files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"),
            QString::fromLocal8Bit("无法打开文件进行写入"));
        return;
    }

    QTextStream out(&file);
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        QString time = ui->tableWidget->item(row, 0)->text();
        QString level = ui->tableWidget->item(row, 1)->text();
        QString tag = ui->tableWidget->item(row, 2)->text();
        QString content = ui->tableWidget->item(row, 3)->text();
        out << QString("[%1] [%2] [%3] %4").arg(time, level, tag, content) << "\n";
    }

    file.close();
    QMessageBox::information(this, QString::fromLocal8Bit("成功"),
        QString::fromLocal8Bit("日志已导出到: %1").arg(fileName));
}

void LogWidget::onLevelFilterChanged()
{
    refreshLogs();
}

void LogWidget::onRefreshLogs()
{
    refreshLogs();
}

void LogWidget::refreshLogs()
{
    if (!m_logManager) {
        return;
    }

    ui->tableWidget->setRowCount(0);

    QList<LogEntry> logs = m_logManager->getLogs(500);

    for (const LogEntry &entry : logs) {
        bool showDebug = ui->checkDebug->isChecked();
        bool showInfo = ui->checkInfo->isChecked();
        bool showWarning = ui->checkWarning->isChecked();
        bool showError = ui->checkError->isChecked();

        bool shouldShow = false;
        switch (entry.level) {
        case LogLevel::DEBUG:   shouldShow = showDebug; break;
        case LogLevel::INFO:    shouldShow = showInfo; break;
        case LogLevel::WARNING: shouldShow = showWarning; break;
        case LogLevel::ERROR:   shouldShow = showError; break;
        }

        if (!shouldShow) {
            continue;
        }

        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        QTableWidgetItem *timeItem = new QTableWidgetItem(entry.timestamp);
        timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 0, timeItem);

        QTableWidgetItem *levelItem = new QTableWidgetItem(levelToString(entry.level));
        levelItem->setFlags(levelItem->flags() & ~Qt::ItemIsEditable);
        levelItem->setTextColor(QColor(getLevelColor(entry.level)));
        ui->tableWidget->setItem(row, 1, levelItem);

        QTableWidgetItem *tagItem = new QTableWidgetItem(entry.tag);
        tagItem->setFlags(tagItem->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 2, tagItem);

        QTableWidgetItem *contentItem = new QTableWidgetItem(entry.message);
        contentItem->setFlags(contentItem->flags() & ~Qt::ItemIsEditable);
        ui->tableWidget->setItem(row, 3, contentItem);
    }
}

QString LogWidget::levelToString(LogLevel level) const
{
    switch (level) {
    case LogLevel::DEBUG:   return "DEBUG";
    case LogLevel::INFO:    return "INFO";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR:   return "ERROR";
    default:                return "UNKNOWN";
    }
}

QString LogWidget::getLevelColor(LogLevel level) const
{
    switch (level) {
    case LogLevel::DEBUG:   return "#808080";
    case LogLevel::INFO:    return "#000000";
    case LogLevel::WARNING: return "#FFA500";
    case LogLevel::ERROR:   return "#FF0000";
    default:                return "#000000";
    }
}

#include "eventwidget.h"
#include "ui_eventwidget.h"
#include "../../Cores/BaseCorePlugin/EventManager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QDateTime>
#include <QPushButton>
#include <QAbstractItemView>
#include <QCheckBox>

EventWidget::EventWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EventWidget)
    , m_eventManager(nullptr)
    , m_refreshTimer(new QTimer(this))
{
    ui->setupUi(this);

    connect(ui->btnClear, &QPushButton::clicked, this, &EventWidget::onClearEvents);
    connect(ui->btnPublish, &QPushButton::clicked, this, &EventWidget::onPublishEvent);
    connect(ui->checkAutoScroll, &QCheckBox::toggled, this, &EventWidget::onAutoScrollToggled);
    connect(m_refreshTimer, &QTimer::timeout, this, &EventWidget::refreshEvents);

    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()
        << QString::fromLocal8Bit("时间") << QString::fromLocal8Bit("事件类型") << QString::fromLocal8Bit("数据内容"));
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_refreshTimer->start(1000);
}

EventWidget::~EventWidget()
{
    m_refreshTimer->stop();
    delete ui;
}

void EventWidget::setEventManager(IBaseEventBus *manager)
{
    m_eventManager = manager;
    CoreEventBus *coreBus = dynamic_cast<CoreEventBus*>(m_eventManager);
    if (coreBus) {
        connect(coreBus, &CoreEventBus::eventPublished,
                this, &EventWidget::onEventPublished);
    }
}

void EventWidget::onEventPublished(const QString &eventType, const QVariantMap &data)
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);

    QTableWidgetItem *timeItem = new QTableWidgetItem(QDateTime::currentDateTime().toString("HH:mm:ss"));
    timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
    ui->tableWidget->setItem(row, 0, timeItem);

    QTableWidgetItem *typeItem = new QTableWidgetItem(eventType);
    typeItem->setFlags(typeItem->flags() & ~Qt::ItemIsEditable);
    ui->tableWidget->setItem(row, 1, typeItem);

    QString dataStr;
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        if (!dataStr.isEmpty()) dataStr += ", ";
        dataStr += QString("%1: %2").arg(it.key(), it.value().toString());
    }
    QTableWidgetItem *dataItem = new QTableWidgetItem(dataStr);
    dataItem->setFlags(dataItem->flags() & ~Qt::ItemIsEditable);
    ui->tableWidget->setItem(row, 2, dataItem);

    if (ui->checkAutoScroll->isChecked()) {
        ui->tableWidget->scrollToBottom();
    }
}

void EventWidget::onClearEvents()
{
    ui->tableWidget->setRowCount(0);
}

void EventWidget::onPublishEvent()
{
    QString eventType = ui->lineEditEventType->text();
    QString dataStr = ui->textEditData->toPlainText();

    if (eventType.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"),
            QString::fromLocal8Bit("请输入事件类型"));
        return;
    }

    QVariantMap data;
    if (!dataStr.isEmpty()) {
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(dataStr.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError && doc.isObject()) {
            data = doc.object().toVariantMap();
        } else {
            data["raw"] = dataStr;
        }
    }

    if (m_eventManager) {
        m_eventManager->publish(eventType, data);
    }
}

void EventWidget::onAutoScrollToggled(bool checked)
{
    Q_UNUSED(checked);
}

void EventWidget::refreshEvents()
{
}

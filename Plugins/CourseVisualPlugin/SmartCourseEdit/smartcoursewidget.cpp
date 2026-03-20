#include "smartcoursewidget.h"
#include "ui_smartcoursewidget.h"
#include <QInputDialog>

SmartCourseWidget::SmartCourseWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SmartCourseWidget)
    , m_largeStepModel(new LargeStepModel(this))
    , m_largeStepDelegate(new LargeStepDelegate(this))
    , m_smallStepModel(new SmallStepModel(this))
    , m_smallStepDelegate(new SmallStepDelegate(this))
    , m_largeItemMenu(nullptr)
    , m_largeEmptyMenu(nullptr)
    , m_smallItemMenu(nullptr)
    , m_smallEmptyMenu(nullptr)
{
    ui->setupUi(this);

    m_largeStepDelegate->setModel(m_largeStepModel);
    ui->largeStepListView->setStepModel(m_largeStepModel);
    ui->largeStepListView->setItemDelegate(m_largeStepDelegate);

    m_smallStepModel->setLargeStepModel(m_largeStepModel);
    m_smallStepDelegate->setModel(m_smallStepModel);
    ui->smallStepListView->setStepModel(m_smallStepModel);
    ui->smallStepListView->setItemDelegate(m_smallStepDelegate);

    setupLargeContextMenus();
    setupLargeEmptyMenu();
    setupSmallContextMenus();
    setupSmallEmptyMenu();

    connect(ui->largeStepListView, &LargeStepListView::customContextMenuRequested,
            this, &SmartCourseWidget::onLargeContextMenuRequested);
    connect(ui->largeStepListView, &LargeStepListView::addItemPressed,
            this, &SmartCourseWidget::onAddLargeItemPressed);
    connect(ui->largeStepListView, &LargeStepListView::addItemClicked,
            this, &SmartCourseWidget::onAddLargeItemClicked);
    connect(ui->largeStepListView, &LargeStepListView::stepSelectionChanged,
            this, &SmartCourseWidget::onLargeStepSelectionChanged);
    connect(m_largeStepModel, &LargeStepModel::stepsChanged,
            this, &SmartCourseWidget::onLargeStepsChanged);

    connect(ui->smallStepListView, &SmallStepListView::customContextMenuRequested,
            this, &SmartCourseWidget::onSmallContextMenuRequested);
    connect(ui->smallStepListView, &SmallStepListView::addItemPressed,
            this, &SmartCourseWidget::onAddSmallItemPressed);
    connect(ui->smallStepListView, &SmallStepListView::addItemClicked,
            this, &SmartCourseWidget::onAddSmallItemClicked);
    connect(ui->smallStepListView, &SmallStepListView::stepSelectionChanged,
            this, &SmartCourseWidget::onSmallStepSelectionChanged);
}

SmartCourseWidget::~SmartCourseWidget()
{
    delete ui;
}

void SmartCourseWidget::setupLargeContextMenus()
{
    m_largeItemMenu = new QMenu(this);
    m_largeItemMenu->addAction(QString::fromLocal8Bit("前面添加步骤"), this, &SmartCourseWidget::onInsertLargeStepBefore);
    m_largeItemMenu->addAction(QString::fromLocal8Bit("后面添加步骤"), this, &SmartCourseWidget::onInsertLargeStepAfter);
    m_largeItemMenu->addSeparator();
    m_largeItemMenu->addAction(QString::fromLocal8Bit("步骤前移"), this, &SmartCourseWidget::onMoveLargeStepUp);
    m_largeItemMenu->addAction(QString::fromLocal8Bit("步骤后移"), this, &SmartCourseWidget::onMoveLargeStepDown);
    m_largeItemMenu->addSeparator();
    m_largeItemMenu->addAction(QString::fromLocal8Bit("修改名称"), this, &SmartCourseWidget::onRenameLargeStep);
    m_largeItemMenu->addAction(QString::fromLocal8Bit("删除"), this, &SmartCourseWidget::onDeleteLargeStep);
    m_largeItemMenu->addSeparator();
    m_largeItemMenu->addAction(QString::fromLocal8Bit("清空所有"), this, &SmartCourseWidget::onClearAllLargeSteps);
}

void SmartCourseWidget::setupLargeEmptyMenu()
{
    m_largeEmptyMenu = new QMenu(this);
    m_largeEmptyMenu->addAction(QString::fromLocal8Bit("在末尾添加步骤"), this, [this]() {
        int addIndex = m_largeStepModel->rowCount() - 1;
        if (addIndex < 0) addIndex = 0;
        onAddLargeItemPressed(addIndex);
    });
}

void SmartCourseWidget::setupSmallContextMenus()
{
    m_smallItemMenu = new QMenu(this);
    m_smallItemMenu->addAction(QString::fromLocal8Bit("前面添加小步骤"), this, &SmartCourseWidget::onInsertSmallStepBefore);
    m_smallItemMenu->addAction(QString::fromLocal8Bit("后面添加小步骤"), this, &SmartCourseWidget::onInsertSmallStepAfter);
    m_smallItemMenu->addSeparator();
    m_smallItemMenu->addAction(QString::fromLocal8Bit("小步骤前移"), this, &SmartCourseWidget::onMoveSmallStepUp);
    m_smallItemMenu->addAction(QString::fromLocal8Bit("小步骤后移"), this, &SmartCourseWidget::onMoveSmallStepDown);
    m_smallItemMenu->addSeparator();
    m_smallItemMenu->addAction(QString::fromLocal8Bit("修改名称"), this, &SmartCourseWidget::onRenameSmallStep);
    m_smallItemMenu->addAction(QString::fromLocal8Bit("删除"), this, &SmartCourseWidget::onDeleteSmallStep);
    m_smallItemMenu->addSeparator();
    m_smallItemMenu->addAction(QString::fromLocal8Bit("资源绑定"), this, &SmartCourseWidget::onResourceBinding);
    m_smallItemMenu->addAction(QString::fromLocal8Bit("规则绑定"), this, &SmartCourseWidget::onRuleBinding);
    m_smallItemMenu->addSeparator();
    m_smallItemMenu->addAction(QString::fromLocal8Bit("清空所有"), this, &SmartCourseWidget::onClearAllSmallSteps);
}

void SmartCourseWidget::setupSmallEmptyMenu()
{
    m_smallEmptyMenu = new QMenu(this);
    m_smallEmptyMenu->addAction(QString::fromLocal8Bit("在末尾添加小步骤"), this, [this]() {
        if (m_smallStepModel->largeStepIndex() < 0) return;
        int addIndex = m_smallStepModel->rowCount() - 1;
        if (addIndex < 0) addIndex = 0;
        onAddSmallItemPressed(addIndex);
    });
}

void SmartCourseWidget::onLargeContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->largeStepListView->indexAt(pos);

    if (index.isValid() && !m_largeStepModel->isAddItem(index.row()) && !m_largeStepModel->isArrow(index.row())) {
        int stepIndex = index.row() / 2;
        m_largeStepModel->setSelectedIndex(stepIndex);
        m_largeItemMenu->exec(ui->largeStepListView->mapToGlobal(pos));
    } else if (!index.isValid()) {
        m_largeEmptyMenu->exec(ui->largeStepListView->mapToGlobal(pos));
    }
}

void SmartCourseWidget::onSmallContextMenuRequested(const QPoint &pos)
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    QModelIndex index = ui->smallStepListView->indexAt(pos);

    if (index.isValid() && !m_smallStepModel->isAddItem(index.row())) {
        m_smallStepModel->setSelectedIndex(index.row());
        m_smallItemMenu->exec(ui->smallStepListView->mapToGlobal(pos));
    } else if (!index.isValid()) {
        m_smallEmptyMenu->exec(ui->smallStepListView->mapToGlobal(pos));
    }
}

void SmartCourseWidget::onInsertLargeStepBefore()
{
    int currentRow = ui->largeStepListView->currentIndex().row();
    if (currentRow < 0) return;

    int stepIndex = currentRow / 2;
    bool ok;
    QString name = QInputDialog::getText(this, QString::fromLocal8Bit("添加步骤"), QString::fromLocal8Bit("步骤名称:"), QLineEdit::Normal, QString::fromLocal8Bit("新步骤"), &ok);
    if (ok && !name.isEmpty()) {
        m_largeStepModel->insertStep(stepIndex, name);
    }
}

void SmartCourseWidget::onInsertLargeStepAfter()
{
    int currentRow = ui->largeStepListView->currentIndex().row();
    if (currentRow < 0) return;

    int stepIndex = currentRow / 2;
    bool ok;
    QString name = QInputDialog::getText(this, QString::fromLocal8Bit("添加步骤"), QString::fromLocal8Bit("步骤名称:"), QLineEdit::Normal, QString::fromLocal8Bit("新步骤"), &ok);
    if (ok && !name.isEmpty()) {
        m_largeStepModel->insertStep(stepIndex + 1, name);
    }
}

void SmartCourseWidget::onMoveLargeStepUp()
{
    int currentRow = ui->largeStepListView->currentIndex().row();
    if (currentRow < 0) return;

    int stepIndex = currentRow / 2;
    if (stepIndex <= 0) return;

    m_largeStepModel->moveStep(stepIndex, stepIndex - 1);
    ui->largeStepListView->setCurrentIndex(m_largeStepModel->index((stepIndex - 1) * 2, 0));
}

void SmartCourseWidget::onMoveLargeStepDown()
{
    int currentRow = ui->largeStepListView->currentIndex().row();
    if (currentRow < 0) return;

    int stepIndex = currentRow / 2;
    int stepCount = m_largeStepModel->rowCount() / 2;
    if (stepIndex >= stepCount - 1) return;

    m_largeStepModel->moveStep(stepIndex, stepIndex + 1);
    ui->largeStepListView->setCurrentIndex(m_largeStepModel->index((stepIndex + 1) * 2, 0));
}

void SmartCourseWidget::onRenameLargeStep()
{
    QModelIndex index = ui->largeStepListView->currentIndex();
    if (!index.isValid()) return;

    ui->largeStepListView->edit(index);
}

void SmartCourseWidget::onDeleteLargeStep()
{
    int currentRow = ui->largeStepListView->currentIndex().row();
    if (currentRow < 0 || m_largeStepModel->isAddItem(currentRow) || m_largeStepModel->isArrow(currentRow)) return;

    int stepIndex = currentRow / 2;
    int oldSmallStepIndex = m_smallStepModel->largeStepIndex();
    m_largeStepModel->removeStep(stepIndex);

    if (oldSmallStepIndex == stepIndex) {
        m_smallStepModel->setLargeStepIndex(-1);
    } else if (oldSmallStepIndex > stepIndex && oldSmallStepIndex > 0) {
        m_smallStepModel->setLargeStepIndex(oldSmallStepIndex - 1);
    }
}

void SmartCourseWidget::onClearAllLargeSteps()
{
    m_largeStepModel->clear();
    m_smallStepModel->setLargeStepIndex(-1);
}

void SmartCourseWidget::onLargeStepsChanged()
{
    emit largeStepsChanged();
}

void SmartCourseWidget::onLargeStepSelectionChanged(int index)
{
    m_largeStepModel->setSelectedIndex(index);
    m_smallStepModel->setLargeStepIndex(index);
}

void SmartCourseWidget::onAddLargeItemPressed(int index)
{
    Q_UNUSED(index)
    int stepCount = m_largeStepModel->rowCount() / 2;
    QString defaultName = QString::fromLocal8Bit("大步骤%1").arg(stepCount + 1);
    m_largeStepModel->insertStep(stepCount, defaultName);
}

void SmartCourseWidget::onAddLargeItemClicked(const QModelIndex &index)
{
    if (m_largeStepModel->isAddItem(index.row())) {
        ui->largeStepListView->edit(index);
    }
}

void SmartCourseWidget::onInsertSmallStepBefore()
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    int currentRow = ui->smallStepListView->currentIndex().row();
    if (currentRow < 0) return;

    bool ok;
    QString name = QInputDialog::getText(this, QString::fromLocal8Bit("添加小步骤"), QString::fromLocal8Bit("小步骤名称:"), QLineEdit::Normal, QString::fromLocal8Bit("新小步骤"), &ok);
    if (ok && !name.isEmpty()) {
        m_largeStepModel->insertSmallStep(m_smallStepModel->largeStepIndex(), currentRow, name);
    }
}

void SmartCourseWidget::onInsertSmallStepAfter()
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    int currentRow = ui->smallStepListView->currentIndex().row();
    if (currentRow < 0) return;

    bool ok;
    QString name = QInputDialog::getText(this, QString::fromLocal8Bit("添加小步骤"), QString::fromLocal8Bit("小步骤名称:"), QLineEdit::Normal, QString::fromLocal8Bit("新小步骤"), &ok);
    if (ok && !name.isEmpty()) {
        m_largeStepModel->insertSmallStep(m_smallStepModel->largeStepIndex(), currentRow + 1, name);
    }
}

void SmartCourseWidget::onMoveSmallStepUp()
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    int currentRow = ui->smallStepListView->currentIndex().row();
    if (currentRow <= 0) return;

    m_largeStepModel->moveSmallStep(m_smallStepModel->largeStepIndex(), currentRow, currentRow - 1);
    ui->smallStepListView->setCurrentIndex(m_smallStepModel->index(currentRow - 1, 0));
}

void SmartCourseWidget::onMoveSmallStepDown()
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    int currentRow = ui->smallStepListView->currentIndex().row();
    int smallCount = m_largeStepModel->smallStepCount(m_smallStepModel->largeStepIndex());
    if (currentRow < 0 || currentRow >= smallCount) return;

    m_largeStepModel->moveSmallStep(m_smallStepModel->largeStepIndex(), currentRow, currentRow + 1);
    ui->smallStepListView->setCurrentIndex(m_smallStepModel->index(currentRow + 1, 0));
}

void SmartCourseWidget::onRenameSmallStep()
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    QModelIndex index = ui->smallStepListView->currentIndex();
    if (!index.isValid()) return;

    ui->smallStepListView->edit(index);
}

void SmartCourseWidget::onDeleteSmallStep()
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    int currentRow = ui->smallStepListView->currentIndex().row();
    if (currentRow < 0 || m_smallStepModel->isAddItem(currentRow)) return;

    m_largeStepModel->removeSmallStep(m_smallStepModel->largeStepIndex(), currentRow);
}

void SmartCourseWidget::onClearAllSmallSteps()
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    int largeIdx = m_smallStepModel->largeStepIndex();
    int count = m_largeStepModel->smallStepCount(largeIdx);
    for (int i = count - 1; i >= 0; --i) {
        m_largeStepModel->removeSmallStep(largeIdx, i);
    }
}

void SmartCourseWidget::onResourceBinding()
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    int currentRow = ui->smallStepListView->currentIndex().row();
    if (currentRow < 0 || m_smallStepModel->isAddItem(currentRow)) return;

    bool ok;
    QString resource = QInputDialog::getText(this, QString::fromLocal8Bit("资源绑定"),
                                             QString::fromLocal8Bit("资源路径:"),
                                             QLineEdit::Normal,
                                             m_smallStepModel->resourceBinding(currentRow),
                                             &ok);
    if (ok) {
        m_largeStepModel->setSmallStepResourceBinding(m_smallStepModel->largeStepIndex(), currentRow, resource);
    }
}

void SmartCourseWidget::onRuleBinding()
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    int currentRow = ui->smallStepListView->currentIndex().row();
    if (currentRow < 0 || m_smallStepModel->isAddItem(currentRow)) return;

    bool ok;
    QString rule = QInputDialog::getText(this, QString::fromLocal8Bit("规则绑定"),
                                         QString::fromLocal8Bit("规则:"),
                                         QLineEdit::Normal,
                                         m_smallStepModel->ruleBinding(currentRow),
                                         &ok);
    if (ok) {
        m_largeStepModel->setSmallStepRuleBinding(m_smallStepModel->largeStepIndex(), currentRow, rule);
    }
}

void SmartCourseWidget::onAddSmallItemPressed(int index)
{
    if (m_smallStepModel->largeStepIndex() < 0) return;

    int stepCount = m_largeStepModel->smallStepCount(m_smallStepModel->largeStepIndex());
    QString defaultName = QString::fromLocal8Bit("小步骤%1").arg(stepCount + 1);
    m_largeStepModel->addSmallStep(m_smallStepModel->largeStepIndex(), defaultName);

    QModelIndex newIndex = m_smallStepModel->index(index, 0);
    ui->smallStepListView->setCurrentIndex(newIndex);
    ui->smallStepListView->edit(newIndex);
}

void SmartCourseWidget::onAddSmallItemClicked(const QModelIndex &index)
{
    if (m_smallStepModel->largeStepIndex() < 0) return;
    if (m_smallStepModel->isAddItem(index.row())) {
        ui->smallStepListView->edit(index);
    }
}

void SmartCourseWidget::onSmallStepSelectionChanged(int index)
{
    m_smallStepModel->setSelectedIndex(index);
}

QStringList SmartCourseWidget::getLargeSteps() const
{
    QStringList steps;
    int stepCount = m_largeStepModel->rowCount() / 2;
    for (int i = 0; i < stepCount; ++i) {
        steps.append(m_largeStepModel->stepAt(i).name);
    }
    return steps;
}

void SmartCourseWidget::setLargeSteps(const QStringList &steps)
{
    m_largeStepModel->clear();
    for (const QString &step : steps) {
        m_largeStepModel->addStep(step);
    }
}

void SmartCourseWidget::clearAll()
{
    m_largeStepModel->clear();
    m_smallStepModel->setLargeStepIndex(-1);
}

#include "addconfigdialog.h"
#include <QInputDialog>
#include <QMessageBox>

AddConfigDialog::AddConfigDialog(const QStringList &groups, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QString::fromLocal8Bit("添加配置"));
    setupUI(groups);
}

AddConfigDialog::~AddConfigDialog()
{
}

void AddConfigDialog::setupUI(const QStringList &groups)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *keyLabel = new QLabel(QString::fromLocal8Bit("配置名称:"), this);
    m_keyEdit = new QLineEdit(this);
    mainLayout->addWidget(keyLabel);
    mainLayout->addWidget(m_keyEdit);

    QLabel *valueLabel = new QLabel(QString::fromLocal8Bit("配置值:"), this);
    m_valueEdit = new QLineEdit(this);
    mainLayout->addWidget(valueLabel);
    mainLayout->addWidget(m_valueEdit);

    QLabel *descLabel = new QLabel(QString::fromLocal8Bit("配置描述:"), this);
    m_descEdit = new QLineEdit(this);
    mainLayout->addWidget(descLabel);
    mainLayout->addWidget(m_descEdit);

    QHBoxLayout *groupLayout = new QHBoxLayout(this);
    QLabel *groupLabel = new QLabel(QString::fromLocal8Bit("配置组:"), this);
    m_groupCombo = new QComboBox(this);
    m_groupCombo->addItem(QString::fromLocal8Bit("(根)"), "");
    for (const QString &group : groups) {
        if (!group.isEmpty() && group != "General") {
            m_groupCombo->addItem(group, group);
        } else if (group == "General") {
            m_groupCombo->addItem(group, group);
        }
    }
    m_addGroupBtn = new QPushButton(QString::fromLocal8Bit("新增组"), this);
    groupLayout->addWidget(groupLabel);
    groupLayout->addWidget(m_groupCombo);
    groupLayout->addWidget(m_addGroupBtn);
    mainLayout->addLayout(groupLayout);

    QHBoxLayout *btnLayout = new QHBoxLayout(this);
    m_okBtn = new QPushButton(QString::fromLocal8Bit("确定"), this);
    m_cancelBtn = new QPushButton(QString::fromLocal8Bit("取消"), this);
    btnLayout->addStretch();
    btnLayout->addWidget(m_okBtn);
    btnLayout->addWidget(m_cancelBtn);
    mainLayout->addLayout(btnLayout);

    connect(m_addGroupBtn, &QPushButton::clicked, this, &AddConfigDialog::onAddGroupClicked);
    connect(m_okBtn, &QPushButton::clicked, this, &AddConfigDialog::onOKClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
}

void AddConfigDialog::onAddGroupClicked()
{
    bool ok;
    QString newGroup = QInputDialog::getText(this, QString::fromLocal8Bit("新增组"),
                                             QString::fromLocal8Bit("请输入新组名称:"),
                                             QLineEdit::Normal, "", &ok);
    if (ok && !newGroup.isEmpty()) {
        m_groupCombo->addItem(newGroup, newGroup);
        m_groupCombo->setCurrentText(newGroup);
    }
}

void AddConfigDialog::onOKClicked()
{
    if (m_keyEdit->text().isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请输入配置名称!"));
        return;
    }
    accept();
}

QString AddConfigDialog::key() const
{
    return m_keyEdit->text();
}

QString AddConfigDialog::value() const
{
    return m_valueEdit->text();
}

QString AddConfigDialog::description() const
{
    return m_descEdit->text();
}

QString AddConfigDialog::group() const
{
    return m_groupCombo->currentData().toString();
}

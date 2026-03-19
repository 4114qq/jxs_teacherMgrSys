#include "themewidget.h"
#include "ui_themewidget.h"
#include "../../common/interfaces/IThemeManager.h"
#include "../../common/interfaces/IBaseEventBus.h"
#include <QDebug>

ThemeWidget::ThemeWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ThemeWidget)
    , m_themeManager(nullptr)
    , m_eventBus(nullptr)
    , m_themeComboBox(nullptr)
    , m_currentThemeLabel(nullptr)
{
    ui->setupUi(this);
    initUI();
}

ThemeWidget::~ThemeWidget()
{
    delete ui;
}

void ThemeWidget::setThemeManager(IThemeManager *themeManager)
{
    m_themeManager = themeManager;
    if (m_themeManager) {
        loadThemes();
    }
}

void ThemeWidget::setEventBus(IBaseEventBus *eventBus)
{
    m_eventBus = eventBus;
}

void ThemeWidget::initUI()
{
    m_themeComboBox = ui->themeComboBox;
    m_currentThemeLabel = ui->currentThemeLabel;

    connect(m_themeComboBox, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &ThemeWidget::onThemeSelected);
}

void ThemeWidget::loadThemes()
{
    if (!m_themeManager) {
        return;
    }

    QStringList themes = m_themeManager->availableThemes();
    m_themeComboBox->clear();
    m_themeComboBox->addItems(themes);

    QString current = m_themeManager->currentTheme();
    int index = m_themeComboBox->findText(current);
    if (index >= 0) {
        m_themeComboBox->setCurrentIndex(index);
    }

    m_currentThemeLabel->setText(current);
}

void ThemeWidget::onThemeSelected(const QString &themeName)
{
    if (!m_themeManager || !m_eventBus) {
        return;
    }

    m_themeManager->setTheme(themeName);
    m_currentThemeLabel->setText(themeName);

    QVariantMap data;
    data["theme"] = themeName;
    m_eventBus->publish("theme.changed", data);

    qDebug() << "Theme changed to:" << themeName;
}

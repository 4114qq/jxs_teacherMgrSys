#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include "../../common/interfaces/IThemeManager.h"
#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QVariantMap>

class IBaseEventBus;

namespace Ui {
class ThemeWidget;
}

class ThemeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeWidget(QWidget *parent = nullptr);
    ~ThemeWidget();

    void setThemeManager(IThemeManager *themeManager);
    void setEventBus(IBaseEventBus *eventBus);

private:
    void initUI();
    void loadThemes();

    Ui::ThemeWidget *ui;
    IThemeManager *m_themeManager;
    IBaseEventBus *m_eventBus;
    QComboBox *m_themeComboBox;
    QLabel *m_currentThemeLabel;

private slots:
    void onThemeSelected(const QString &themeName);
};

#endif

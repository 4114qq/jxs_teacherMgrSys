#ifndef HTTPDATAWIDGET_H
#define HTTPDATAWIDGET_H

#include <QWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "../../common/interfaces/IHttpClientManager.h"
#include "../../common/interfaces/IConfigManager.h"
#include "../../common/definitions/HttpDataStructures.h"

QT_BEGIN_NAMESPACE
namespace Ui { class HttpDataWidget; }
QT_END_NAMESPACE

class HttpDataWidget : public QWidget
{
    Q_OBJECT

public:
    HttpDataWidget(QWidget *parent = nullptr);
    ~HttpDataWidget();

    void setHttpClientManager(IHttpClientManager *manager);
    void setConfigManager(IConfigManager *configManager);

private slots:
    void onCourseFetchClicked();
    void onCourseUploadClicked();
    void onCourseAddClicked();

    void onEquipmentFetchClicked();
    void onEquipmentUploadClicked();
    void onEquipmentAddClicked();
    void onEquipmentDeleteClicked();

    void onTestConnectionClicked();
    void onSaveConfigClicked();

    void onCourseResponse(const QJsonObject &response);
    void onEquipmentResponse(const QJsonObject &response);
    void onUploadResponse(const QJsonObject &response);

private:
    void initServerConfig();
    void loadConfig();

    Ui::HttpDataWidget *ui;
    IHttpClientManager *m_httpClientManager;
    IConfigManager *m_configManager;

    QTabWidget *m_tabWidget;
    QLineEdit *m_serverIpEdit;
    QSpinBox *m_serverPortSpin;
    QLineEdit *m_tokenEdit;
    QSpinBox *m_timeoutSpin;
    QLabel *m_statusLabel;
    QLabel *m_errorLabel;

    QTextEdit *m_courseTextEdit;
    QTextEdit *m_equipmentTextEdit;
    QTextEdit *m_logTextEdit;

    QList<MKCourseInfo> m_courseList;
    QList<EquipmentLine> m_equipmentList;
};
#endif // HTTPDATAWIDGET_H

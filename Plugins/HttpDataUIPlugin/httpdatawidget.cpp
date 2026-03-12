#include "httpdatawidget.h"
#include "ui_httpdatawidget.h"
#include <QDateTime>
#include <QInputDialog>

HttpDataWidget::HttpDataWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HttpDataWidget)
    , m_httpClientManager(nullptr)
    , m_configManager(nullptr)
{
    ui->setupUi(this);

    m_serverIpEdit = ui->serverIpEdit;
    m_serverPortSpin = ui->serverPortSpin;
    m_tokenEdit = ui->tokenEdit;
    m_timeoutSpin = ui->timeoutSpin;
    m_statusLabel = ui->statusLabel;

    m_courseTextEdit = ui->courseTextEdit;
    m_equipmentTextEdit = ui->equipmentTextEdit;
    m_logTextEdit = ui->logTextEdit;

    connect(ui->btnTestConnection, &QPushButton::clicked, this, &HttpDataWidget::onTestConnectionClicked);
    connect(ui->btnSaveConfig, &QPushButton::clicked, this, &HttpDataWidget::onSaveConfigClicked);
    connect(ui->btnCourseAdd, &QPushButton::clicked, this, &HttpDataWidget::onCourseAddClicked);
    connect(ui->btnCourseFetch, &QPushButton::clicked, this, &HttpDataWidget::onCourseFetchClicked);
    connect(ui->btnCourseUpload, &QPushButton::clicked, this, &HttpDataWidget::onCourseUploadClicked);
    connect(ui->btnEquipmentAdd, &QPushButton::clicked, this, &HttpDataWidget::onEquipmentAddClicked);
    connect(ui->btnEquipmentFetch, &QPushButton::clicked, this, &HttpDataWidget::onEquipmentFetchClicked);
    connect(ui->btnEquipmentUpload, &QPushButton::clicked, this, &HttpDataWidget::onEquipmentUploadClicked);

    loadConfig();
}

HttpDataWidget::~HttpDataWidget()
{
    delete ui;
}

void HttpDataWidget::setHttpClientManager(IHttpClientManager *manager)
{
    m_httpClientManager = manager;
}

void HttpDataWidget::setConfigManager(IConfigManager *configManager)
{
    m_configManager = configManager;
    loadConfig();
}

void HttpDataWidget::loadConfig()
{
    if (!m_configManager) {
        return;
    }

    QString ip = m_configManager->get("HTTP.IP", "192.168.1.10").toString();
    int port = m_configManager->get("HTTP.Port", 8094).toInt();
    QString token = m_configManager->get("HTTP.Token", "").toString();
    int timeout = m_configManager->get("HTTP.Timeout", 5).toInt();

    m_serverIpEdit->setText(ip);
    m_serverPortSpin->setValue(port);
    m_tokenEdit->setText(token);
    m_timeoutSpin->setValue(timeout);
}

void HttpDataWidget::initServerConfig()
{
    if (!m_httpClientManager) {
        return;
    }

    QString ip = m_serverIpEdit->text();
    int port = m_serverPortSpin->value();
    m_httpClientManager->setServerUrl(ip, port);

    QString token = m_tokenEdit->text();
    m_httpClientManager->setToken(token);

    int timeout = m_timeoutSpin->value();
    m_httpClientManager->setTimeout(timeout);

    m_statusLabel->setText(QString::fromLocal8Bit("已连接"));
    m_statusLabel->setStyleSheet("color: green;");
}

void HttpDataWidget::onTestConnectionClicked()
{
    if (!m_httpClientManager) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("HTTP客户端管理器未初始化"));
        return;
    }

    initServerConfig();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(QString::fromLocal8Bit("开始测试连接...")));

    m_httpClientManager->get("/TMS/api/course/list", QJsonObject(), [this](const QJsonObject &response) {
        onCourseResponse(response);
    });
}

void HttpDataWidget::onSaveConfigClicked()
{
    if (!m_configManager) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("配置管理器未初始化"));
        return;
    }

    QString ip = m_serverIpEdit->text();
    int port = m_serverPortSpin->value();
    QString token = m_tokenEdit->text();
    int timeout = m_timeoutSpin->value();

    m_configManager->set("HTTP.IP", ip);
    m_configManager->set("HTTP.Port", port);
    m_configManager->set("HTTP.Token", token);
    m_configManager->set("HTTP.Timeout", timeout);

    if (m_configManager->save()) {
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("配置保存成功!"));
    } else {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("配置保存失败!"));
    }
}

void HttpDataWidget::onCourseAddClicked()
{
    QString jsonStr = m_courseTextEdit->toPlainText();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        m_courseTextEdit->setPlainText("[\n  {\n    \"courseGuid\": \"\",\n    \"courseName\": \"\",\n    \"courseDesc\": \"\",\n    \"categoryId\": \"\",\n    \"creatorId\": \"\",\n    \"createTime\": \"\",\n    \"modifierId\": \"\",\n    \"modifyTime\": \"\",\n    \"modifyMode\": \"\",\n    \"attribute\": {\n      \"fitType\": \"\",\n      \"workItemTags\": \"\",\n      \"workItemTagsName\": \"\",\n      \"abilityTags\": \"\",\n      \"abilityTagsName\": \"\",\n      \"vehicle\": {\n        \"vehicleType\": \"\",\n        \"applicableModel\": \"\",\n        \"deviceLevel\": \"\",\n        \"signalMode\": \"\",\n        \"lines\": [\n          {\n            \"lineCode\": \"\",\n            \"lineName\": \"\"\n          }\n        ]\n      }\n    }\n  }\n]");
    } else {
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            QJsonObject newCourse;
            newCourse["courseGuid"] = "";
            newCourse["courseName"] = "";
            newCourse["courseDesc"] = "";
            newCourse["categoryId"] = "";
            newCourse["creatorId"] = "";
            newCourse["createTime"] = "";
            newCourse["modifierId"] = "";
            newCourse["modifyTime"] = "";
            newCourse["modifyMode"] = "";
            newCourse["attribute"] = QJsonObject();
            arr.append(newCourse);
            m_courseTextEdit->setPlainText(QJsonDocument(arr).toJson(QJsonDocument::Indented));
        }
    }
}

void HttpDataWidget::onCourseFetchClicked()
{
    if (!m_httpClientManager) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("HTTP客户端管理器未初始化"));
        return;
    }

    initServerConfig();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(QString::fromLocal8Bit("正在从服务器获取课程列表...")));

    m_httpClientManager->get("/TMS/api/course/list", QJsonObject(), [this](const QJsonObject &response) {
        onCourseResponse(response);
    });
}

void HttpDataWidget::onCourseUploadClicked()
{
    if (!m_httpClientManager) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("HTTP客户端管理器未初始化"));
        return;
    }

    initServerConfig();

    QString jsonStr = m_courseTextEdit->toPlainText();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("JSON格式错误: ") + error.errorString());
        return;
    }

    if (!doc.isArray()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("请输入JSON数组格式"));
        return;
    }

    QJsonArray coursesArray = doc.array();
    QList<MKCourseInfo> courses;

    for (const auto &item : coursesArray) {
        if (item.isObject()) {
            courses.append(MKCourseInfo::fromJson(item.toObject()));
        }
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(QString::fromLocal8Bit("正在上传课程...")));

    bool success = m_httpClientManager->uploadCourse(courses);

    if (success) {
        m_logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(QString::fromLocal8Bit("课程上传成功!")));
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("课程上传成功!"));
    } else {
        QString errMsg = m_httpClientManager->lastError();
        m_logTextEdit->append(QString("[%1] %2: %3").arg(timestamp).arg(QString::fromLocal8Bit("课程上传失败")).arg(errMsg));
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("课程上传失败: ") + errMsg);
    }
}

void HttpDataWidget::onEquipmentAddClicked()
{
    QString jsonStr = m_equipmentTextEdit->toPlainText();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        m_equipmentTextEdit->setPlainText("[\n  {\n    \"deviceId\": \"\",\n    \"lines\": [\n      {\n        \"lineCode\": \"\",\n        \"lineName\": \"\",\n        \"deployDate\": \"\"\n      }\n    ]\n  }\n]");
    } else {
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            QJsonObject newEquipment;
            newEquipment["deviceId"] = "";
            newEquipment["lines"] = QJsonArray();
            arr.append(newEquipment);
            m_equipmentTextEdit->setPlainText(QJsonDocument(arr).toJson(QJsonDocument::Indented));
        }
    }
}

void HttpDataWidget::onEquipmentFetchClicked()
{
    if (!m_httpClientManager) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("HTTP客户端管理器未初始化"));
        return;
    }

    initServerConfig();

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(QString::fromLocal8Bit("正在从服务器获取设备列表...")));

    m_httpClientManager->get("/TMS/api/equipment/line/list", QJsonObject(), [this](const QJsonObject &response) {
        onEquipmentResponse(response);
    });
}

void HttpDataWidget::onEquipmentUploadClicked()
{
    if (!m_httpClientManager) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("HTTP客户端管理器未初始化"));
        return;
    }

    initServerConfig();

    QString jsonStr = m_equipmentTextEdit->toPlainText();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);

    if (error.error != QJsonParseError::NoError) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("JSON格式错误: ") + error.errorString());
        return;
    }

    if (!doc.isArray()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("请输入JSON数组格式"));
        return;
    }

    QJsonArray equipmentArray = doc.array();
    QList<EquipmentLine> equipmentLines;

    for (const auto &item : equipmentArray) {
        if (item.isObject()) {
            equipmentLines.append(EquipmentLine::fromJson(item.toObject()));
        }
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    m_logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(QString::fromLocal8Bit("正在上传设备线路...")));

    bool success = m_httpClientManager->uploadEquipmentLine(equipmentLines);

    if (success) {
        m_logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(QString::fromLocal8Bit("设备线路上传成功!")));
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("设备线路上传成功!"));
    } else {
        QString errMsg = m_httpClientManager->lastError();
        m_logTextEdit->append(QString("[%1] %2: %3").arg(timestamp).arg(QString::fromLocal8Bit("设备线路上传失败")).arg(errMsg));
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("设备线路上传失败: ") + errMsg);
    }
}

void HttpDataWidget::onEquipmentDeleteClicked()
{
}

void HttpDataWidget::onCourseResponse(const QJsonObject &response)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    int code = response["code"].toInt();
    if (code == 0) {
        QJsonObject data = response["data"].toObject();
        QJsonArray coursesArray = data["courses"].toArray();

        QJsonArray outputArray;
        for (const auto &item : coursesArray) {
            outputArray.append(item.toObject());
        }

        m_courseTextEdit->setPlainText(QJsonDocument(outputArray).toJson(QJsonDocument::Indented));
        m_logTextEdit->append(QString("[%1] %2: %3").arg(timestamp).arg(QString::fromLocal8Bit("获取课程成功")).arg(QString::number(coursesArray.size()) + QString::fromLocal8Bit(" 条记录")));
    } else {
        QString message = response["message"].toString();
        m_logTextEdit->append(QString("[%1] %2: %3").arg(timestamp).arg(QString::fromLocal8Bit("获取课程失败")).arg(message));
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("获取课程失败: ") + message);
    }
}

void HttpDataWidget::onEquipmentResponse(const QJsonObject &response)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    int code = response["code"].toInt();
    if (code == 0) {
        QJsonArray array = response["data"].toArray();

        m_equipmentTextEdit->setPlainText(QJsonDocument(array).toJson(QJsonDocument::Indented));
        m_logTextEdit->append(QString("[%1] %2: %3").arg(timestamp).arg(QString::fromLocal8Bit("获取设备列表成功")).arg(QString::number(array.size()) + QString::fromLocal8Bit(" 条记录")));
    } else {
        QString message = response["message"].toString();
        m_logTextEdit->append(QString("[%1] %2: %3").arg(timestamp).arg(QString::fromLocal8Bit("获取设备列表失败")).arg(message));
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("获取设备列表失败: ") + message);
    }
}

void HttpDataWidget::onUploadResponse(const QJsonObject &response)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    int code = response["code"].toInt();
    if (code == 0) {
        m_logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(QString::fromLocal8Bit("上传成功!")));
    } else {
        QString message = response["message"].toString();
        m_logTextEdit->append(QString("[%1] %2: %3").arg(timestamp).arg(QString::fromLocal8Bit("上传失败")).arg(message));
    }
}

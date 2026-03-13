/**
 * @file HttpClientManager.cpp
 * @brief HTTP客户端管理器实现
 * @details 提供HTTP请求、认证管理、课程和设备数据同步等功能实现
 */

#include "HttpClientManager.h"
#include <QDebug>
#include <QDateTime>
#include <QEventLoop>

// ===== 构造函数 =====

HttpClientManager::HttpClientManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_timeout(5)
    , m_lastStatusCode(0)
    , m_lastSuccess(false)
    , m_timeoutTimer(new QTimer(this))
    , m_currentReply(nullptr)
{
    connect(m_timeoutTimer, &QTimer::timeout, this, &HttpClientManager::onTimeout);
}

HttpClientManager::~HttpClientManager()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
}

// ===== 服务器配置 =====

void HttpClientManager::setServerUrl(const QString& url)
{
    m_serverUrl = url;
}

void HttpClientManager::setServerUrl(const QString& ip, int port)
{
    m_serverUrl = QString("http://%1:%2").arg(ip).arg(port);
}

QString HttpClientManager::serverUrl() const
{
    return m_serverUrl;
}

// ===== 超时设置 =====

void HttpClientManager::setTimeout(int seconds)
{
    m_timeout = seconds;
}

int HttpClientManager::timeout() const
{
    return m_timeout;
}

// ===== 认证管理 =====

void HttpClientManager::setToken(const QString& token)
{
    m_token = token;
}

QString HttpClientManager::getToken() const
{
    return m_token;
}

bool HttpClientManager::isLoggedIn() const
{
    return !m_token.isEmpty();
}

bool HttpClientManager::login(const QString& username, const QString& password)
{
    QJsonObject data;
    data["username"] = username;
    data["password"] = password;

    QJsonObject response;
    if (!sendRequest("/TMS/api/auth/login", data, response)) {
        return false;
    }

    if (response["code"].toInt() == 0) {
        m_token = response["data"].toObject()["token"].toString();
        m_lastSuccess = true;
        return true;
    }

    m_lastError = response["message"].toString();
    m_lastSuccess = false;
    return false;
}

bool HttpClientManager::loginWithToken(const QString& token)
{
    m_token = token;
    return isLoggedIn();
}

void HttpClientManager::logout()
{
    m_token.clear();
}

bool HttpClientManager::refreshToken()
{
    QJsonObject data;
    data["token"] = m_token;

    QJsonObject response;
    if (!sendRequest("/TMS/api/auth/refresh", data, response)) {
        return false;
    }

    if (response["code"].toInt() == 0) {
        m_token = response["data"].toObject()["token"].toString();
        return true;
    }

    return false;
}

// ===== 设备线路操作 =====

bool HttpClientManager::uploadEquipmentLine(const QList<EquipmentLine>& lines)
{
    QJsonObject data;
    QJsonArray linesArray;
    for (const auto& line : lines) {
        linesArray.append(line.toJson());
    }
    data["lines"] = linesArray;

    QJsonObject response;
    if (!sendRequest("/TMS/api/equipment/line/upload", data, response)) {
        return false;
    }

    m_lastSuccess = (response["code"].toInt() == 0);
    if (!m_lastSuccess) {
        m_lastError = response["message"].toString();
    }
    return m_lastSuccess;
}

bool HttpClientManager::deleteEquipmentLine(const QString& deviceId)
{
    QJsonObject data;
    data["deviceId"] = deviceId;

    QJsonObject response;
    if (!sendRequest("/TMS/api/equipment/line/delete", data, response)) {
        return false;
    }

    m_lastSuccess = (response["code"].toInt() == 0);
    if (!m_lastSuccess) {
        m_lastError = response["message"].toString();
    }
    return m_lastSuccess;
}

// ===== 课程操作 =====

bool HttpClientManager::uploadCourse(const QList<MKCourseInfo>& courses, const OverhaulItem* overhaulItem)
{
    QJsonObject data;
    QJsonArray coursesArray;
    for (const auto& course : courses) {
        coursesArray.append(course.toJson());
    }
    data["courses"] = coursesArray;

    if (overhaulItem && overhaulItem->isValid()) {
        data["overhaulItem"] = overhaulItem->toJson();
    }

    QJsonObject response;
    if (!sendRequest("/TMS/api/course/upload", data, response)) {
        return false;
    }

    m_lastSuccess = (response["code"].toInt() == 0);
    if (!m_lastSuccess) {
        m_lastError = response["message"].toString();
    }
    return m_lastSuccess;
}

bool HttpClientManager::deleteCourse(const QList<CourseInfo>& courses)
{
    QJsonObject data;
    QJsonArray coursesArray;
    for (const auto& course : courses) {
        coursesArray.append(course.toJson());
    }
    data["courses"] = coursesArray;

    QJsonObject response;
    if (!sendRequest("/TMS/api/course/delete", data, response)) {
        return false;
    }

    m_lastSuccess = (response["code"].toInt() == 0);
    if (!m_lastSuccess) {
        m_lastError = response["message"].toString();
    }
    return m_lastSuccess;
}

QList<MKCourseInfo> HttpClientManager::fetchCourseList()
{
    QList<MKCourseInfo> result;

    QJsonObject response;
    if (!sendRequest("/TMS/api/course/list", QJsonObject(), response)) {
        return result;
    }

    if (response["code"].toInt() == 0) {
        QJsonArray coursesArray = response["data"].toObject()["courses"].toArray();
        for (const auto& item : coursesArray) {
            result.append(MKCourseInfo::fromJson(item.toObject()));
        }
    }

    return result;
}

MKCourseInfo HttpClientManager::fetchCourse(const QString& courseGuid)
{
    MKCourseInfo result;

    QJsonObject params;
    params["courseGuid"] = courseGuid;

    QJsonObject response;
    if (!sendRequest("/TMS/api/course/detail", params, response)) {
        return result;
    }

    if (response["code"].toInt() == 0) {
        result = MKCourseInfo::fromJson(response["data"].toObject());
    }

    return result;
}

// ===== 错误信息 =====

QString HttpClientManager::lastError() const
{
    return m_lastError;
}

int HttpClientManager::lastStatusCode() const
{
    return m_lastStatusCode;
}

bool HttpClientManager::lastResponseSuccess() const
{
    return m_lastSuccess;
}

// ===== 通用请求 =====

void HttpClientManager::get(const QString& apiPath, const QJsonObject& params, std::function<void(QJsonObject)> callback)
{
    QString url = getFullUrl(apiPath);
    if (!params.isEmpty()) {
        QString query;
        for (auto it = params.begin(); it != params.end(); ++it) {
            if (!query.isEmpty()) query += "&";
            query += QString("%1=%2").arg(it.key()).arg(it.value().toString());
        }
        url += "?" + query;
    }

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_token).toUtf8());
    }

    m_pendingCallback = callback;
    m_timeoutTimer->start(m_timeout * 1000);

    m_currentReply = m_networkManager->get(request);
    connect(m_currentReply, &QNetworkReply::finished, this, &HttpClientManager::onNetworkReply);
}

void HttpClientManager::post(const QString& apiPath, const QJsonObject& data, std::function<void(QJsonObject)> callback)
{
    QNetworkRequest request;
    request.setUrl(QUrl(getFullUrl(apiPath)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_token).toUtf8());
    }

    m_pendingCallback = callback;
    m_timeoutTimer->start(m_timeout * 1000);

    QJsonDocument doc(data);
    m_currentReply = m_networkManager->post(request, doc.toJson());
    connect(m_currentReply, &QNetworkReply::finished, this, &HttpClientManager::onNetworkReply);
}

void HttpClientManager::onNetworkReply()
{
    m_timeoutTimer->stop();

    if (!m_currentReply) {
        return;
    }

    m_lastStatusCode = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (m_currentReply->error() != QNetworkReply::NoError) {
        handleError(m_currentReply->error());
        if (m_pendingCallback) {
            QJsonObject errorObj;
            errorObj["code"] = -1;
            errorObj["message"] = m_lastError;
            m_pendingCallback(errorObj);
        }
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
        return;
    }

    QByteArray responseData = m_currentReply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonObject response = doc.object();

    if (m_lastStatusCode == 401) {
        if (refreshToken()) {
            m_lastError = "Token refreshed";
        } else {
            m_lastError = "Token expired";
            logout();
        }
    }

    if (m_pendingCallback) {
        m_pendingCallback(response);
    }

    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void HttpClientManager::onTimeout()
{
    m_timeoutTimer->stop();
    m_lastError = "Request timeout";
    m_lastStatusCode = -1;

    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }

    if (m_pendingCallback) {
        QJsonObject errorObj;
        errorObj["code"] = -1;
        errorObj["message"] = m_lastError;
        m_pendingCallback(errorObj);
    }
}

bool HttpClientManager::sendRequest(const QString& apiPath, const QJsonObject& data, QJsonObject& response)
{
    QEventLoop loop;
    bool completed = false;
    bool success = false;

    QNetworkRequest request;
    request.setUrl(QUrl(getFullUrl(apiPath)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    if (!m_token.isEmpty()) {
        request.setRawHeader("Authorization", QString("Bearer %1").arg(m_token).toUtf8());
    }

    QNetworkReply* reply = nullptr;
    if (data.isEmpty()) {
        reply = m_networkManager->get(request);
    } else {
        QJsonDocument doc(data);
        reply = m_networkManager->post(request, doc.toJson());
    }

    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    timeoutTimer.start(m_timeout * 1000);

    QObject::connect(reply, &QNetworkReply::finished, &loop, [&]() {
        completed = true;
    });

    QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, [&]() {
        if (!completed) {
            reply->abort();
        }
    });

    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        m_lastStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        response = doc.object();
        success = (response["code"].toInt() == 0);
    } else {
        m_lastError = reply->errorString();
        m_lastStatusCode = -1;
    }

    reply->deleteLater();
    return success;
}

void HttpClientManager::handleError(QNetworkReply::NetworkError error)
{
    switch (error) {
    case QNetworkReply::ConnectionRefusedError:
        m_lastError = "Connection refused";
        break;
    case QNetworkReply::RemoteHostClosedError:
        m_lastError = "Remote host closed";
        break;
    case QNetworkReply::HostNotFoundError:
        m_lastError = "Host not found";
        break;
    case QNetworkReply::TimeoutError:
        m_lastError = "Request timeout";
        break;
    case QNetworkReply::SslHandshakeFailedError:
        m_lastError = "SSL handshake failed";
        break;
    default:
        m_lastError = "Network error";
        break;
    }
    qWarning() << "HttpClient error:" << m_lastError;
}

QString HttpClientManager::getFullUrl(const QString& apiPath) const
{
    QString url = m_serverUrl;
    if (!url.endsWith('/') && !apiPath.startsWith('/')) {
        url += "/";
    } else if (url.endsWith('/') && apiPath.startsWith('/')) {
        url = url.left(url.length() - 1);
    }
    return url + apiPath;
}

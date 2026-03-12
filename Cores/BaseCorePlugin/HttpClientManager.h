#ifndef HTTPCLIENTMANAGER_H
#define HTTPCLIENTMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMutex>
#include "../../common/interfaces/IHttpClientManager.h"

class HttpClientManager : public QObject, public IHttpClientManager
{
    Q_OBJECT
    Q_INTERFACES(IHttpClientManager)

public:
    explicit HttpClientManager(QObject *parent = nullptr);
    ~HttpClientManager() override;

    void setServerUrl(const QString& url) override;
    void setServerUrl(const QString& ip, int port) override;
    QString serverUrl() const override;

    void setTimeout(int seconds) override;
    int timeout() const override;

    void setToken(const QString& token) override;
    QString getToken() const override;
    bool isLoggedIn() const override;

    bool login(const QString& username, const QString& password) override;
    bool loginWithToken(const QString& token) override;
    void logout() override;
    bool refreshToken() override;

    bool uploadEquipmentLine(const QList<EquipmentLine>& lines) override;
    bool deleteEquipmentLine(const QString& deviceId) override;

    bool uploadCourse(const QList<MKCourseInfo>& courses, const OverhaulItem* overhaulItem = nullptr) override;
    bool deleteCourse(const QList<CourseInfo>& courses) override;
    QList<MKCourseInfo> fetchCourseList() override;
    MKCourseInfo fetchCourse(const QString& courseGuid) override;

    QString lastError() const override;
    int lastStatusCode() const override;
    bool lastResponseSuccess() const override;

    void get(const QString& apiPath, const QJsonObject& params, std::function<void(QJsonObject)> callback) override;
    void post(const QString& apiPath, const QJsonObject& data, std::function<void(QJsonObject)> callback) override;

private slots:
    void onNetworkReply();
    void onTimeout();

private:
    bool sendRequest(const QString& apiPath, const QJsonObject& data, QJsonObject& response);
    void handleError(QNetworkReply::NetworkError error);
    QString getFullUrl(const QString& apiPath) const;

    QNetworkAccessManager* m_networkManager;
    QString m_serverUrl;
    QString m_token;
    int m_timeout;
    int m_lastStatusCode;
    QString m_lastError;
    bool m_lastSuccess;
    QMutex m_mutex;
    QTimer* m_timeoutTimer;
    QNetworkReply* m_currentReply;
    std::function<void(QJsonObject)> m_pendingCallback;
};

#endif // HTTPCLIENTMANAGER_H

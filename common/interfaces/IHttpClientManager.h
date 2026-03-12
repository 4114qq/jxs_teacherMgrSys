#ifndef IHTTPCLIENTMANAGER_H
#define IHTTPCLIENTMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QJsonObject>
#include <functional>
#include "../definitions/HttpDataStructures.h"

class IHttpClientManager
{
public:
    virtual ~IHttpClientManager() = default;

    virtual void setServerUrl(const QString& url) = 0;
    virtual void setServerUrl(const QString& ip, int port) = 0;
    virtual QString serverUrl() const = 0;

    virtual void setTimeout(int seconds) = 0;
    virtual int timeout() const = 0;

    virtual void setToken(const QString& token) = 0;
    virtual QString getToken() const = 0;
    virtual bool isLoggedIn() const = 0;

    virtual bool login(const QString& username, const QString& password) = 0;
    virtual bool loginWithToken(const QString& token) = 0;
    virtual void logout() = 0;
    virtual bool refreshToken() = 0;

    virtual bool uploadEquipmentLine(const QList<EquipmentLine>& lines) = 0;
    virtual bool deleteEquipmentLine(const QString& deviceId) = 0;

    virtual bool uploadCourse(const QList<MKCourseInfo>& courses, const OverhaulItem* overhaulItem = nullptr) = 0;
    virtual bool deleteCourse(const QList<CourseInfo>& courses) = 0;
    virtual QList<MKCourseInfo> fetchCourseList() = 0;
    virtual MKCourseInfo fetchCourse(const QString& courseGuid) = 0;

    virtual QString lastError() const = 0;
    virtual int lastStatusCode() const = 0;
    virtual bool lastResponseSuccess() const = 0;

    virtual void get(const QString& apiPath, const QJsonObject& params, std::function<void(QJsonObject)> callback) = 0;
    virtual void post(const QString& apiPath, const QJsonObject& data, std::function<void(QJsonObject)> callback) = 0;
};

#define IHTTPCLIENTMANAGER_IID "com.jxs.teacherMgrSys.IHttpClientManager"
Q_DECLARE_INTERFACE(IHttpClientManager, IHTTPCLIENTMANAGER_IID)

#endif // IHTTPCLIENTMANAGER_H

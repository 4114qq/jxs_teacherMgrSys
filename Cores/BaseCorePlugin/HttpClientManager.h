/**
 * @file HttpClientManager.h
 * @brief HTTP客户端管理器实现
 * @details 提供HTTP请求、认证管理、课程和设备数据同步等功能
 */

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

/**
 * @class HttpClientManager
 * @brief HTTP客户端管理器
 * @details 实现HTTP请求、认证管理、课程和设备数据同步等功能
 */
class HttpClientManager : public QObject, public IHttpClientManager
{
    Q_OBJECT
    Q_INTERFACES(IHttpClientManager)

public:
    // ===== 构造函数 =====

    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit HttpClientManager(QObject *parent = nullptr);

    /**
     * @brief 析构函数
     */
    ~HttpClientManager() override;

    // ===== 服务器配置 =====

    /**
     * @brief 设置服务器URL
     * @param url 服务器URL
     */
    void setServerUrl(const QString& url) override;

    /**
     * @brief 设置服务器地址
     * @param ip 服务器IP
     * @param port 端口号
     */
    void setServerUrl(const QString& ip, int port) override;

    /**
     * @brief 获取服务器URL
     * @return QString 服务器URL
     */
    QString serverUrl() const override;

    // ===== 超时设置 =====

    /**
     * @brief 设置请求超时时间
     * @param seconds 超时秒数
     */
    void setTimeout(int seconds) override;

    /**
     * @brief 获取请求超时时间
     * @return int 超时秒数
     */
    int timeout() const override;

    // ===== 认证管理 =====

    /**
     * @brief 设置认证令牌
     * @param token 令牌
     */
    void setToken(const QString& token) override;

    /**
     * @brief 获取认证令牌
     * @return QString 令牌
     */
    QString getToken() const override;

    /**
     * @brief 检查是否已登录
     * @return bool 是否已登录
     */
    bool isLoggedIn() const override;

    /**
     * @brief 用户名密码登录
     * @param username 用户名
     * @param password 密码
     * @return bool 登录是否成功
     */
    bool login(const QString& username, const QString& password) override;

    /**
     * @brief 使用令牌登录
     * @param token 令牌
     * @return bool 登录是否成功
     */
    bool loginWithToken(const QString& token) override;

    /**
     * @brief 登出
     */
    void logout() override;

    /**
     * @brief 刷新令牌
     * @return bool 刷新是否成功
     */
    bool refreshToken() override;

    // ===== 设备线路操作 =====

    /**
     * @brief 上传设备线路
     * @param lines 设备线路列表
     * @return bool 上传是否成功
     */
    bool uploadEquipmentLine(const QList<EquipmentLine>& lines) override;

    /**
     * @brief 删除设备线路
     * @param deviceId 设备ID
     * @return bool 删除是否成功
     */
    bool deleteEquipmentLine(const QString& deviceId) override;

    // ===== 课程操作 =====

    /**
     * @brief 上传课程
     * @param courses 课程列表
     * @param overhaulItem 检修项目
     * @return bool 上传是否成功
     */
    bool uploadCourse(const QList<MKCourseInfo>& courses, const OverhaulItem* overhaulItem = nullptr) override;

    /**
     * @brief 删除课程
     * @param courses 课程列表
     * @return bool 删除是否成功
     */
    bool deleteCourse(const QList<CourseInfo>& courses) override;

    /**
     * @brief 获取课程列表
     * @return QList<MKCourseInfo> 课程列表
     */
    QList<MKCourseInfo> fetchCourseList() override;

    /**
     * @brief 获取课程详情
     * @param courseGuid 课程GUID
     * @return MKCourseInfo 课程信息
     */
    MKCourseInfo fetchCourse(const QString& courseGuid) override;

    // ===== 错误信息 =====

    /**
     * @brief 获取最后一次错误信息
     * @return QString 错误信息
     */
    QString lastError() const override;

    /**
     * @brief 获取最后一次HTTP状态码
     * @return int HTTP状态码
     */
    int lastStatusCode() const override;

    /**
     * @brief 检查最后一次响应是否成功
     * @return bool 是否成功
     */
    bool lastResponseSuccess() const override;

    // ===== 通用请求 =====

    /**
     * @brief 发送GET请求
     * @param apiPath API路径
     * @param params 请求参数
     * @param callback 回调函数
     */
    void get(const QString& apiPath, const QJsonObject& params, std::function<void(QJsonObject)> callback) override;

    /**
     * @brief 发送POST请求
     * @param apiPath API路径
     * @param data 请求数据
     * @param callback 回调函数
     */
    void post(const QString& apiPath, const QJsonObject& data, std::function<void(QJsonObject)> callback) override;

private slots:
    /**
     * @brief 网络响应处理槽函数
     */
    void onNetworkReply();

    /**
     * @brief 超时处理槽函数
     */
    void onTimeout();

private:
    /**
     * @brief 发送请求
     * @param apiPath API路径
     * @param data 请求数据
     * @param response 响应数据
     * @return bool 请求是否成功
     */
    bool sendRequest(const QString& apiPath, const QJsonObject& data, QJsonObject& response);

    /**
     * @brief 处理网络错误
     * @param error 错误类型
     */
    void handleError(QNetworkReply::NetworkError error);

    /**
     * @brief 获取完整URL
     * @param apiPath API路径
     * @return QString 完整URL
     */
    QString getFullUrl(const QString& apiPath) const;

private:
    QNetworkAccessManager* m_networkManager;  ///< 网络访问管理器
    QString m_serverUrl;                      ///< 服务器URL
    QString m_token;                          ///< 认证令牌
    int m_timeout;                           ///< 超时秒数
    int m_lastStatusCode;                   ///< 最后一次HTTP状态码
    QString m_lastError;                     ///< 最后一次错误信息
    bool m_lastSuccess;                      ///< 最后一次请求是否成功
    QMutex m_mutex;                          ///< 互斥锁
    QTimer* m_timeoutTimer;                 ///< 超时定时器
    QNetworkReply* m_currentReply;           ///< 当前网络响应
    std::function<void(QJsonObject)> m_pendingCallback; ///< 待处理的回调函数
};

#endif // HTTPCLIENTMANAGER_H

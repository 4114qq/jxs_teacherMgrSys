/**
 * @file IHttpClientManager.h
 * @brief HTTP客户端管理接口定义
 * @details 定义HTTP请求、认证、数据上传下载等操作的接口
 */

#ifndef IHTTPCLIENTMANAGER_H
#define IHTTPCLIENTMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QJsonObject>
#include <functional>
#include "../definitions/HttpDataStructures.h"

/**
 * @class IHttpClientManager
 * @brief HTTP客户端管理器接口
 * @details 提供HTTP请求、认证管理、课程和设备数据同步等功能
 */
class IHttpClientManager
{
public:
    /**
     * @brief 析构函数
     */
    virtual ~IHttpClientManager() = default;

    // ===== 服务器配置 =====

    /**
     * @brief 设置服务器URL
     * @param url 完整URL地址
     */
    virtual void setServerUrl(const QString& url) = 0;

    /**
     * @brief 设置服务器地址
     * @param ip 服务器IP地址
     * @param port 服务器端口
     */
    virtual void setServerUrl(const QString& ip, int port) = 0;

    /**
     * @brief 获取服务器URL
     * @return QString 服务器URL
     */
    virtual QString serverUrl() const = 0;

    // ===== 超时设置 =====

    /**
     * @brief 设置请求超时时间
     * @param seconds 超时秒数
     */
    virtual void setTimeout(int seconds) = 0;

    /**
     * @brief 获取请求超时时间
     * @return int 超时秒数
     */
    virtual int timeout() const = 0;

    // ===== 认证管理 =====

    /**
     * @brief 设置认证Token
     * @param token 认证令牌
     */
    virtual void setToken(const QString& token) = 0;

    /**
     * @brief 获取认证Token
     * @return QString 认证令牌
     */
    virtual QString getToken() const = 0;

    /**
     * @brief 检查是否已登录
     * @return bool 是否已登录
     */
    virtual bool isLoggedIn() const = 0;

    /**
     * @brief 用户名密码登录
     * @param username 用户名
     * @param password 密码
     * @return bool 登录是否成功
     */
    virtual bool login(const QString& username, const QString& password) = 0;

    /**
     * @brief Token登录
     * @param token 认证令牌
     * @return bool 登录是否成功
     */
    virtual bool loginWithToken(const QString& token) = 0;

    /**
     * @brief 登出
     */
    virtual void logout() = 0;

    /**
     * @brief 刷新Token
     * @return bool 刷新是否成功
     */
    virtual bool refreshToken() = 0;

    // ===== 设备线路操作 =====

    /**
     * @brief 上传设备线路
     * @param lines 设备线路列表
     * @return bool 上传是否成功
     */
    virtual bool uploadEquipmentLine(const QList<EquipmentLine>& lines) = 0;

    /**
     * @brief 删除设备线路
     * @param deviceId 设备ID
     * @return bool 删除是否成功
     */
    virtual bool deleteEquipmentLine(const QString& deviceId) = 0;

    // ===== 课程操作 =====

    /**
     * @brief 上传课程
     * @param courses 课程列表
     * @param overhaulItem 检修项目（可选）
     * @return bool 上传是否成功
     */
    virtual bool uploadCourse(const QList<MKCourseInfo>& courses, const OverhaulItem* overhaulItem = nullptr) = 0;

    /**
     * @brief 删除课程
     * @param courses 课程列表
     * @return bool 删除是否成功
     */
    virtual bool deleteCourse(const QList<CourseInfo>& courses) = 0;

    /**
     * @brief 获取课程列表
     * @return QList<MKCourseInfo> 课程列表
     */
    virtual QList<MKCourseInfo> fetchCourseList() = 0;

    /**
     * @brief 获取课程详情
     * @param courseGuid 课程GUID
     * @return MKCourseInfo 课程信息
     */
    virtual MKCourseInfo fetchCourse(const QString& courseGuid) = 0;

    // ===== 错误信息 =====

    /**
     * @brief 获取最近错误信息
     * @return QString 错误信息
     */
    virtual QString lastError() const = 0;

    /**
     * @brief 获取最近HTTP状态码
     * @return int HTTP状态码
     */
    virtual int lastStatusCode() const = 0;

    /**
     * @brief 检查最近响应是否成功
     * @return bool 响应是否成功
     */
    virtual bool lastResponseSuccess() const = 0;

    // ===== 通用请求 =====

    /**
     * @brief 发送GET请求
     * @param apiPath API路径
     * @param params 请求参数
     * @param callback 回调函数
     */
    virtual void get(const QString& apiPath, const QJsonObject& params, std::function<void(QJsonObject)> callback) = 0;

    /**
     * @brief 发送POST请求
     * @param apiPath API路径
     * @param data 请求数据
     * @param callback 回调函数
     */
    virtual void post(const QString& apiPath, const QJsonObject& data, std::function<void(QJsonObject)> callback) = 0;
};

#define IHTTPCLIENTMANAGER_IID "com.jxs.teacherMgrSys.IHttpClientManager"
Q_DECLARE_INTERFACE(IHttpClientManager, IHTTPCLIENTMANAGER_IID)

#endif // IHTTPCLIENTMANAGER_H

/**
 * @file IBaseEventBus.h
 * @brief 事件总线接口定义
 * @details 定义事件发布/订阅机制的接口
 */

#ifndef IBASEEVENTBUS_H
#define IBASEEVENTBUS_H

#include <QVariantMap>

/**
 * @class IBaseEventBus
 * @brief 事件总线接口
 * @details 提供事件发布和订阅功能，用于组件间的解耦通信
 */
class IBaseEventBus
{
public:
    /**
     * @brief 析构函数
     */
    virtual ~IBaseEventBus() = default;

    // ===== 事件发布 =====

    /**
     * @brief 发布事件
     * @param eventType 事件类型标识
     * @param data 事件数据
     * @return bool 发布是否成功
     */
    virtual bool publish(const QString& eventType,
                        const QVariantMap& data = QVariantMap()) = 0;

    // ===== 事件订阅 =====

    /**
     * @brief 订阅事件
     * @param eventType 事件类型标识
     * @param receiver 接收者对象
     * @param slot 槽函数指针
     */
    virtual void subscribe(const QString& eventType, 
                          QObject* receiver, 
                          const char* slot) = 0;

    /**
     * @brief 取消订阅
     * @param eventType 事件类型标识
     * @param receiver 接收者对象
     */
    virtual void unsubscribe(const QString& eventType, 
                            QObject* receiver) = 0;
};

#endif // IBASEEVENTBUS_H

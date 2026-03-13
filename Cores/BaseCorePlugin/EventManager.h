/**
 * @file EventManager.h
 * @brief 事件管理器实现
 * @details 实现事件发布和订阅功能，用于组件间的解耦通信
 */

#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H

#include <QObject>
#include <QVariantMap>
#include "../../common/interfaces/IBaseEventBus.h"
#include "BaseCorePlugin_global.h"

/**
 * @class CoreEventBus
 * @brief 核心事件总线
 * @details 实现事件的发布和订阅机制
 */
class BASECOREPLUGIN_EXPORT CoreEventBus : public QObject, public IBaseEventBus
{
    Q_OBJECT

public:
    // ===== 事件发布 =====

    /**
     * @brief 发布事件
     * @param eventType 事件类型标识
     * @param data 事件数据
     * @return bool 发布是否成功
     */
    bool publish(const QString& eventType,
                const QVariantMap& data = QVariantMap()) override;

    // ===== 事件订阅 =====

    /**
     * @brief 订阅事件
     * @param eventType 事件类型标识
     * @param receiver 接收者对象
     * @param slot 槽函数指针
     */
    void subscribe(const QString& eventType,
                  QObject* receiver,
                  const char* slot) override;

    /**
     * @brief 取消订阅
     * @param eventType 事件类型标识
     * @param receiver 接收者对象
     */
    void unsubscribe(const QString& eventType,
                    QObject* receiver) override;

signals:
    /**
     * @brief 事件发布信号
     * @param eventType 事件类型标识
     * @param data 事件数据
     */
    void eventPublished(const QString &eventType, const QVariantMap &data);

private:
    /**
     * @struct Subscription
     * @brief 订阅结构
     */
    struct Subscription {
        QObject* receiver;  ///< 接收者对象
        const char* slot;   ///< 槽函数指针
    };

    QMap<QString, QList<Subscription>> m_subscriptions; ///< 订阅列表
};

#endif // EVENTMANAGER_H

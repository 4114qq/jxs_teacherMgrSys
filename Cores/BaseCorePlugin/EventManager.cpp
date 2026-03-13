/**
 * @file EventManager.cpp
 * @brief 事件管理器实现
 * @details 实现事件发布和订阅功能实现
 */

#include "EventManager.h"

// ===== 事件发布 =====

bool CoreEventBus::publish(const QString& eventType,
                          const QVariantMap& data)
{
    emit eventPublished(eventType, data);

    if (!m_subscriptions.contains(eventType)) return false;

    const auto& subs = m_subscriptions[eventType];
    for (const auto& sub : subs) {
        QMetaObject::invokeMethod(sub.receiver, sub.slot,
                                 Q_ARG(QVariantMap, data));
    }

    return true;
}

// ===== 事件订阅 =====

void CoreEventBus::subscribe(const QString& eventType,
                            QObject* receiver,
                            const char* slot)
{
    Subscription sub = { receiver, slot };
    m_subscriptions[eventType].append(sub);
}

void CoreEventBus::unsubscribe(const QString& eventType,
                              QObject* receiver)
{
    if (!m_subscriptions.contains(eventType)) return;

    auto& subs = m_subscriptions[eventType];
    for (int i = subs.size() - 1; i >= 0; --i) {
        if (subs[i].receiver == receiver) {
            subs.removeAt(i);
        }
    }
}

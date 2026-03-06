#ifndef IEVENTMANAGER_H
#define IEVENTMANAGER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <functional>

/**
 * @brief 事件管理器接口
 * 负责管理插件间的事件通信
 */
class IEventManager
{
public:
    virtual ~IEventManager() = default;

    // 事件订阅
    virtual bool subscribe(const QString &eventName, std::function<bool(const QVariantMap&)> handler) = 0;
    virtual bool unsubscribe(const QString &eventName) = 0;

    // 事件触发
    virtual bool triggerEvent(const QString &eventName, const QVariantMap &eventData) = 0;

    // 事件管理
    virtual QStringList getRegisteredEvents() const = 0;
    virtual int getSubscriberCount(const QString &eventName) const = 0;
};

#define IEVENTMANAGER_IID "com.jxs.teacherMgrSys.IEventManager"
Q_DECLARE_INTERFACE(IEventManager, IEVENTMANAGER_IID)

#endif // IEVENTMANAGER_H

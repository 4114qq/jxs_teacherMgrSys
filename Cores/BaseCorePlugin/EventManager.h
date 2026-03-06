#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include <QObject>
#include <QVariantMap>
#include "interfaces/IBaseEventBus.h"

class CoreEventBus : public QObject, public IBaseEventBus
{
    Q_OBJECT
    
public:
    bool publish(const QString& eventType,
                const QVariantMap& data = QVariantMap()) override;
    void subscribe(const QString& eventType, 
                  QObject* receiver, 
                  const char* slot) override;
    void unsubscribe(const QString& eventType, 
                    QObject* receiver) override;

private:
    struct Subscription {
        QObject* receiver;
        const char* slot;
    };
    QMap<QString, QList<Subscription>> m_subscriptions;
};
#endif // EVENTMANAGER_H

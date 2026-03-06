#ifndef IBASEEVENTBUS_H
#define IBASEEVENTBUS_H

#include <QVariantMap>

class IBaseEventBus
{
public:
    virtual ~IBaseEventBus() = default;
    
    // 发布事件
    virtual bool publish(const QString& eventType,
                        const QVariantMap& data = QVariantMap()) = 0;
    
    // 订阅事件
    virtual void subscribe(const QString& eventType, 
                          QObject* receiver, 
                          const char* slot) = 0;
    
    // 取消订阅
    virtual void unsubscribe(const QString& eventType, 
                            QObject* receiver) = 0;
};

#endif // IBASEEVENTBUS_H

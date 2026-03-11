#ifndef IBASEPLUGIN_H
#define IBASEPLUGIN_H

#include "IBaseEventBus.h"
#include "IDatabaseManager.h"
#include "IConfigManager.h"
#include "ILogManager.h"
#include "IAuthManager.h"
#include <QObject>
#include <QString>
#include <QVariant>
#include <QWidget>

class IBasePlugin
{
public:
    virtual ~IBasePlugin() = default;

    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual QString description() const = 0;
    virtual QString author() const = 0;

    virtual bool initialize() = 0;
    virtual bool startPlugin() = 0;
    virtual bool stopPlugin() = 0;
    virtual void cleanup() = 0;

    virtual QVariant getConfig(const QString &key, const QVariant &defaultValue = QVariant()) const = 0;
    virtual void setConfig(const QString &key, const QVariant &value) = 0;
    virtual bool loadConfig() = 0;
    virtual bool saveConfig() = 0;

    virtual IBaseEventBus *eventManager() const = 0;
    virtual IDatabaseManager *databaseManager() const = 0;
    virtual ILogManager *logManager() const = 0;
    virtual IAuthManager *authManager() const = 0;
    virtual IConfigManager *configManager() const = 0;
    virtual QStringList dependencies() const = 0;

    virtual void setCore(IBasePlugin *core) = 0;
    virtual IBasePlugin *core() const = 0;
};

#define IBASEPLUGIN_IID "com.jxs.teacherMgrSys.IBasePlugin"

Q_DECLARE_INTERFACE(IBasePlugin, IBASEPLUGIN_IID)

#endif

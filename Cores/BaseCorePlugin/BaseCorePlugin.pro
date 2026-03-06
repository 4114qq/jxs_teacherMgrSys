QT       += core sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += BASECOREPLUGIN_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ConfigManager.cpp \
    DatabaseManager.cpp \
    EventManager.cpp \
    basecoreplugin.cpp

HEADERS += \
    BaseCorePlugin_global.h \
    ConfigManager.h \
    DatabaseManager.h \
    EventManager.h \
    basecoreplugin.h

# Include common directory
INCLUDEPATH += ../../common

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

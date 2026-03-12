QT       += core sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += BASECOREPLUGIN_LIBRARY

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AuthManager.cpp \
    ConfigManager.cpp \
    DatabaseManager.cpp \
    EventManager.cpp \
    HttpClientManager.cpp \
    LogManager.cpp \
    basecoreplugin.cpp

HEADERS += \
    AuthManager.h \
    BaseCorePlugin_global.h \
    ConfigManager.h \
    DatabaseManager.h \
    EventManager.h \
    HttpClientManager.h \
    LogManager.h \
    basecoreplugin.h

# Include common directory
INCLUDEPATH += $$PWD/../../common
include($$PWD/../../common/definitions/definitions.pri)

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
else: win32: target.path = $$PWD/../../bin/x86_64/plugins
!isEmpty(target.path): INSTALLS += target

# Set output directory
DESTDIR = $$PWD/../../bin/x86_64/plugins

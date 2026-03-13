QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += CONFIGUIPLUGIN_LIBRARY

CONFIG += plugin c++17

TARGET = $$qtLibraryTarget(ConfigUIPlugin)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    configuiplugin.cpp \
    configwidget.cpp

HEADERS += \
    configuiplugin.h \
    configwidget.h

FORMS += \
    configwidget.ui

# Include common directory
INCLUDEPATH += $$PWD/../../common

# Include BaseCorePlugin directory for ConfigManager
INCLUDEPATH += $$PWD/../../Cores/BaseCorePlugin

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
else: win32: target.path = $$PWD/../../bin/x86_64/plugins
!isEmpty(target.path): INSTALLS += target

# Set output directory
DESTDIR = $$PWD/../../bin/x86_64/plugins

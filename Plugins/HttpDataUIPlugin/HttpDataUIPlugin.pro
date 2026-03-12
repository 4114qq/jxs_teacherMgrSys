QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += plugin

CONFIG += c++17

SOURCES += \
    httpdatauiplugin.cpp \
    httpdatawidget.cpp

HEADERS += \
    httpdatauiplugin.h \
    httpdatawidget.h

FORMS += \
    httpdatawidget.ui

INCLUDEPATH += $$PWD/../../common
INCLUDEPATH += $$PWD/../../Cores/BaseCorePlugin

unix {
    target.path = /usr/lib
}
else: win32: target.path = $$PWD/../../bin/x86_64/plugins
!isEmpty(target.path): INSTALLS += target

DESTDIR = $$PWD/../../bin/x86_64/plugins

QT += core gui sql widgets

TARGET = UserAuthUIPlugin
TEMPLATE = lib

DEFINES += USERAUTHUIPLUGIN_LIBRARY

CONFIG += plugin

DESTDIR = $$PWD/../../bin/x86_64/plugins

INCLUDEPATH += ../../common/interfaces \
                ../BaseCorePlugin

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = $$qtLibraryTarget(UserAuthUIPlugin)

HEADERS += userauthuiplugin.h \
           userauthwidget.h

SOURCES += userauthuiplugin.cpp \
            userauthwidget.cpp

FORMS += userauthwidget.ui


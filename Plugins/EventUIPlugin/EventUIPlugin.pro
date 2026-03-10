QT += core gui sql widgets

TARGET = EventUIPlugin
TEMPLATE = lib

DEFINES += EVENTUIPLUGIN_LIBRARY

CONFIG += plugin

DESTDIR = $$PWD/../../bin/x86_64/plugins

INCLUDEPATH += ../../common/interfaces \
                ../BaseCorePlugin

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = $$qtLibraryTarget(EventUIPlugin)

HEADERS += eventuiplugin.h \
           eventwidget.h

SOURCES += eventuiplugin.cpp \
            eventwidget.cpp

FORMS += eventwidget.ui

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
} else {
    TARGET = $$join(TARGET,,,)
}

win32 {
    LIBS += $$PWD/../../bin/x86_64/plugins/BaseCorePlugin.lib
}

QT += core gui sql widgets

TARGET = LogUIPlugin
TEMPLATE = lib

DEFINES += LOGUIPLUGIN_LIBRARY

CONFIG += plugin

DESTDIR = $$PWD/../../bin/x86_64/plugins

INCLUDEPATH += ../../common/interfaces \
                ../ConfigUIPlugin

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = $$qtLibraryTarget(LogUIPlugin)

HEADERS += loguiplugin.h \
           logwidget.h

SOURCES += loguiplugin.cpp \
            logwidget.cpp

FORMS += logwidget.ui

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
} else {
    TARGET = $$join(TARGET,,,)
}

QT += core gui sql widgets

TARGET = DatabaseUIPlugin
TEMPLATE = lib

DEFINES += DATABASEUIPLUGIN_LIBRARY

CONFIG += plugin

DESTDIR = $$PWD/../../bin/x86_64/plugins

INCLUDEPATH += ../../common/interfaces \
                ../ConfigUIPlugin

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = $$qtLibraryTarget(DatabaseUIPlugin)

HEADERS += databaseuiplugin.h \
           databasewidget.h

SOURCES += databaseuiplugin.cpp \
            databasewidget.cpp

FORMS += databasewidget.ui

CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
} else {
    TARGET = $$join(TARGET,,,)
}

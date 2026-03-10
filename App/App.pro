QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    pluginmanager.cpp

HEADERS += \
    mainwindow.h \
    pluginmanager.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
else: win32: target.path = ../bin/x86_64
!isEmpty(target.path): INSTALLS += target

# Set output directory
DESTDIR = $$PWD/../bin/x86_64

# Include common directory
INCLUDEPATH += $$PWD/../common
INCLUDEPATH += $$PWD/../Plugins/ConfigUIPlugin
INCLUDEPATH += $$PWD/../Plugins/DatabaseUIPlugin
INCLUDEPATH += $$PWD/../Plugins/EventUIPlugin

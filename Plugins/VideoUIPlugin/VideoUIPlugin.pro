QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += VIDEOUIPLUGIN_LIBRARY

CONFIG += plugin c++17

TARGET = $$qtLibraryTarget(VideoUIPlugin)

SOURCES += \
    videouiplugin.cpp \
    videowidget.cpp \
    videocontrolbar.cpp \
    videocentercontrols.cpp

HEADERS += \
    videouiplugin.h \
    videowidget.h \
    videocontrolbar.h \
    videocentercontrols.h

INCLUDEPATH += ../../common/interfaces \
                ../BaseCorePlugin

win32 {
    INCLUDEPATH += $$PWD/../../dependence/x86_64/vlc/include
    LIBS += $$PWD/../../dependence/x86_64/vlc/lib/libvlc.lib $$PWD/../../dependence/x86_64/vlc/lib/libvlccore.lib
}

DESTDIR = $$PWD/../../bin/x86_64/plugins

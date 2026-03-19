QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
DEFINES += COURSEVISUALPLUGIN_LIBRARY

CONFIG += plugin c++17

TARGET = $$qtLibraryTarget(CourseVisualPlugin)

SOURCES += \
    coursevisualplugin.cpp \
    coursevisualwidget.cpp \
    SmartCourseEdit/smartcoursewidget.cpp \
    SmartCourseEdit/LargeStep/largestepmodel.cpp \
    SmartCourseEdit/LargeStep/largestepdelegate.cpp \
    SmartCourseEdit/LargeStep/largesteplistview.cpp \
    SmartCourseEdit/SmallStep/smallstepmodel.cpp \
    SmartCourseEdit/SmallStep/smallstepdelegate.cpp \
    SmartCourseEdit/SmallStep/smallsteplistview.cpp

HEADERS += \
    coursevisualplugin.h \
    coursevisualwidget.h \
    SmartCourseEdit/smartcoursewidget.h \
    SmartCourseEdit/LargeStep/largestepitem.h \
    SmartCourseEdit/LargeStep/largestepmodel.h \
    SmartCourseEdit/LargeStep/largestepdelegate.h \
    SmartCourseEdit/LargeStep/largesteplistview.h \
    SmartCourseEdit/SmallStep/smallstepmodel.h \
    SmartCourseEdit/SmallStep/smallstepdelegate.h \
    SmartCourseEdit/SmallStep/smallsteplistview.h

FORMS += \
    coursevisualwidget.ui \
    SmartCourseEdit/smartcoursewidget.ui

INCLUDEPATH += $$PWD/../../common

DEPENDPATH += $$PWD/../../common

DESTDIR = $$PWD/../../bin/x86_64/plugins

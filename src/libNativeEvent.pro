TEMPLATE = lib

QT =
QT       -= core gui
CONFIG   += console
CONFIG   -= app_bundle

CONFIG *= nativeevent-buildlib
#var with '_' can not pass to pri?
STATICLINK = 0
PROJECTROOT = $$PWD/..
!include(libNativeEvent.pri): error(could not find libNativeEvent.pri)
preparePaths($$OUT_PWD/../out)

#src
unix: SOURCES += 
else:win32: SOURCES += 

SOURCES += \
    NativeEvent.cpp

HEADERS += \
    NativeEvent.h \
    NativeEvent_global.h




TEMPLATE = app


TARGET = tst_nativeevent
STATICLINK = 0
PROJECTROOT = $$PWD/..
include($$PROJECTROOT/src/libNativeEvent.pri)
preparePaths($$OUT_PWD/../out)

#win32:LIBS += -lUser32

SOURCES += main.cpp
HEADERS += 



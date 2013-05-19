TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = libnativeevent test

libnativeevent.file = src/libNativeEvent.pro
test.file = test/tst_NativeEvent.pro
test.depends += depends libnativeevent

OTHER_FILES += README

EssentialDepends =
OptionalDepends =

include(root.pri)

include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp

QT+=xml

TARGET      = $$qtLibraryTarget(xlettestplugin)

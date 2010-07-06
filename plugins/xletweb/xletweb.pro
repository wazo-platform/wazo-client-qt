include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp

TARGET      = $$qtLibraryTarget(xletwebplugin)

QT += webkit

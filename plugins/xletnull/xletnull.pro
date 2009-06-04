include(../../common.pri)
TEMPLATE    = lib
CONFIG      += plugin
INCLUDEPATH += ../../baselib
HEADERS     = xletnull.h   xletnullplugin.h
SOURCES     = xletnull.cpp xletnullplugin.cpp
TARGET      = $$qtLibraryTarget(xletnullplugin)
DESTDIR     = ../../bin/plugins
unix:LIBS   += ../../bin/libxivoclientbaselib.so
win32:LIBS  += ../../bin/libxivoclientbaselib1.a

# QT +=

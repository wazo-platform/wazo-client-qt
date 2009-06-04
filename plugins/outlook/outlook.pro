include(../../common.pri)
TEMPLATE    = lib
CONFIG      += plugin
INCLUDEPATH += ../../baselib
HEADERS     = outlook_*.h   outlookplugin.h
SOURCES     = outlook_*.cpp outlookplugin.cpp
TARGET      = $$qtLibraryTarget(outlookplugin)
DESTDIR     = ../../bin/plugins
unix:LIBS   += ../../bin/libxivoclientbaselib.so
win32:LIBS  += ../../bin/libxivoclientbaselib1.a
win32:LIBS  += -luuid -lole32 -loleaut32

QT += network


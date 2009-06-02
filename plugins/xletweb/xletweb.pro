include(../../common.pri)
TEMPLATE    = lib
CONFIG      += plugin
INCLUDEPATH += ../../common ../../baselib
HEADERS     = xletweb.h   xletwebplugin.h
SOURCES     = xletweb.cpp xletwebplugin.cpp
TARGET      = $$qtLibraryTarget(xletwebplugin)
DESTDIR     = ../../bin/plugins
unix:LIBS   += ../../bin/libxivoclientbaselib.so
win32:LIBS  += ../../bin/libxivoclientbaselib1.a

QT += network
QT += webkit


TEMPLATE    = lib
CONFIG      += plugin
INCLUDEPATH += ../../common ../../baselib
HEADERS     = xletweb.h   xletwebplugin.h
SOURCES     = xletweb.cpp xletwebplugin.cpp
TARGET      = $$qtLibraryTarget(xletwebplugin)
DESTDIR     = ../../bin/plugins
LIBS        += ../../bin/libxivoclientbaselib.so

QT += network
QT += webkit


TEMPLATE    = lib
CONFIG      += plugin
INCLUDEPATH += ../../common ../../baselib
HEADERS     = xletweb.h xletwebplugin.h
# HEADERS     += ../../common/xlet.h
SOURCES     = xletweb.cpp xletwebplugin.cpp
# SOURCES     += ../../common/xlet.cpp
TARGET      = $$qtLibraryTarget(xletwebplugin)
DESTDIR     = ../../bin/plugins
LIBS        += ../../bin/libxivoclientbaselib.so

QT += network
QT += webkit


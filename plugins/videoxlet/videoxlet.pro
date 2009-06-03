include(../../common.pri)
TEMPLATE    = lib
CONFIG      += plugin
INCLUDEPATH += ../../baselib
HEADERS     = videopanel.h   videoxletplugin.h
SOURCES     = videopanel.cpp videoxletplugin.cpp
TARGET      = $$qtLibraryTarget(videoxletplugin)
DESTDIR     = ../../bin/plugins
unix:LIBS   += ../../bin/libxivoclientbaselib.so
win32:LIBS  += ../../bin/libxivoclientbaselib1.a

# QT +=

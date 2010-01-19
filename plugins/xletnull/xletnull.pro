include(../../common.pri)
TEMPLATE    = lib
CONFIG      += plugin

win32 {
  CONFIG -= plugin
  CONFIG += dll
}


INCLUDEPATH += ../../baselib
HEADERS     = xletnull.h   xletnullplugin.h
SOURCES     = xletnull.cpp xletnullplugin.cpp
TARGET      = $$qtLibraryTarget(xletnullplugin)
DESTDIR     = ../../bin/plugins
LIBS         += -L../../bin/
unix:LIBS    += -lxivoclientbaselib
win32:LIBS   += -lxivoclientbaselib1

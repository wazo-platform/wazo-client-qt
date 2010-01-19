include(../../common.pri)
TEMPLATE     = lib
CONFIG += plugin

win32 {
  CONFIG -= plugin
  CONFIG += dll
}

INCLUDEPATH  += ../../baselib
HEADERS      = xletweb.h   xletwebplugin.h
SOURCES      = xletweb.cpp xletwebplugin.cpp
TARGET       = $$qtLibraryTarget(xletwebplugin)
DESTDIR      = ../../bin/plugins
LIBS         += -L../../bin/
unix:LIBS    += -lxivoclientbaselib
win32:LIBS   += -lxivoclientbaselib1

QT += network
QT += webkit


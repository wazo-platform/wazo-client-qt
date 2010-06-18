include(../../common.pri)
TEMPLATE    = lib
CONFIG      += plugin
QT += network

win32 {
  CONFIG -= plugin
  CONFIG += dll
}

INCLUDEPATH += ../../baselib
HEADERS     = videopanel.h   videoxletplugin.h
SOURCES     = videopanel.cpp videoxletplugin.cpp
TARGET      = $$qtLibraryTarget(videoxletplugin)
DESTDIR     = ../../bin/plugins

LIBS         += -L../../bin/
unix:LIBS    += -lxivoclientbaselib
win32:LIBS   += -lxivoclientbaselib1


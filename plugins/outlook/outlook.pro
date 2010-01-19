include(../../common.pri)
TEMPLATE    = lib
CONFIG      += plugin

win32 {
  CONFIG -= plugin
  CONFIG += dll
}

INCLUDEPATH += ../../baselib
HEADERS     = outlook_*.h   outlookplugin.h
SOURCES     = outlook_*.cpp outlookplugin.cpp
TARGET      = $$qtLibraryTarget(outlookplugin)
DESTDIR     = ../../bin/plugins
LIBS        += -L../../bin/
unix:LIBS   += -lxivoclientbaselib
win32 {
  LIBS      += -lxivoclientbaselib1
  LIBS      += -luuid -lole32 -loleaut32
}

QT += network sql

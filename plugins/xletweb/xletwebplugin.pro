TEMPLATE    = lib
CONFIG      += plugin
INCLUDEPATH += ../../common
HEADERS     = xletweb.h xletwebplugin.h ../../common/xlet.h
SOURCES     = xletweb.cpp xletwebplugin.cpp ../../common/xlet.cpp
TARGET      = $$qtLibraryTarget(xletwebplugin)
DESTDIR     = ..

QT += network
QT += webkit

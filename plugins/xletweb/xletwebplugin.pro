TEMPLATE    = lib
CONFIG      += plugin
INCLUDEPATH += ../../common
HEADERS     = xletweb.h xletwebplugin.h
SOURCES     = xletweb.cpp xletwebplugin.cpp
TARGET      = $$qtLibraryTarget(xletwebplugin)
DESTDIR     = ..

QT += network

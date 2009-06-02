TEMPLATE    = lib
CONFIG      += plugin
INCLUDEPATH += ../../common
HEADERS     = xletweb.h xletwebplugin.h
HEADERS     += ../../common/xlet.h
SOURCES     = xletweb.cpp xletwebplugin.cpp
SOURCES     += ../../common/xlet.cpp
TARGET      = $$qtLibraryTarget(xletwebplugin)
DESTDIR     = ..

QT += network
QT += webkit

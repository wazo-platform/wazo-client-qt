# $Id$
TEMPLATE    = lib
CONFIG      += dll
COMMONDIR = ../common
JSON_JSONQT_DIR = ../3rdparty/json_jsonqt/lib

include(../common.pri)

# Library version
VERSION     = 1.1

INCLUDEPATH += $${COMMONDIR} $${JSON_JSONQT_DIR}
HEADERS     = xlet.h   baseengine.h
HEADERS     += phoneinfo.h   userinfo.h
HEADERS     += agentinfo.h   queueinfo.h
HEADERS     += extendedtablewidget.h
HEADERS     += xivoconsts.h
HEADERS     += $${JSON_JSONQT_DIR}/*.h
SOURCES     = xlet.cpp baseengine.cpp
SOURCES     += phoneinfo.cpp userinfo.cpp
SOURCES     += agentinfo.cpp queueinfo.cpp
SOURCES     += extendedtablewidget.cpp
SOURCES     += xivoconsts.cpp
SOURCES     += $${JSON_JSONQT_DIR}/*.cpp

DEFINES     += BASELIB_LIBRARY

DESTDIR     = ../bin

QT += network

TRANSLATIONS = baselib_fr.ts


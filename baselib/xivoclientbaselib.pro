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
HEADERS     += *info.h
HEADERS     += queue_agent_status.h
HEADERS     += extendedtablewidget.h
HEADERS     += xivoconsts.h
HEADERS     += $${JSON_JSONQT_DIR}/*.h
SOURCES     = xlet.cpp baseengine.cpp
SOURCES     += *info.cpp
SOURCES     += queue_agent_status.cpp
SOURCES     += extendedtablewidget.cpp
SOURCES     += xivoconsts.cpp
SOURCES     += $${JSON_JSONQT_DIR}/*.cpp

DEFINES     += BASELIB_LIBRARY

DESTDIR     = ../bin

QT += network

TRANSLATIONS = baselib_fr.ts

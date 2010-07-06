# $Id$
TEMPLATE    = lib
CONFIG      += dll
COMMONDIR = ../common
JSON_JSONQT_DIR = ../3rdparty/json_jsonqt/lib

include(../common.pri)

# Library version
VERSION     = $${XIVOVER}
DEFINES     += BASELIB_LIBRARY

INCLUDEPATH += $${COMMONDIR} $${JSON_JSONQT_DIR} src

HEADERS     += src/*.h
HEADERS     += src/dstore/src/*.h
HEADERS     += $${JSON_JSONQT_DIR}/*.h

SOURCES     += src/*.cpp
SOURCES     += src/dstore/src/*.cpp
SOURCES     += $${JSON_JSONQT_DIR}/*.cpp

DESTDIR     = ../bin
DEFINES     += BASELIB_LIBRARY

QT += network

TRANSLATIONS = baselib_fr.ts

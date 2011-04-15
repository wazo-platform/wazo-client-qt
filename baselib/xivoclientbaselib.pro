# $Id: xivoclientbaselib.pro 9722 2010-12-09 09:53:51Z kaou $
TEMPLATE    = lib
CONFIG      += dll
COMMONDIR = ../common
JSON_JSONQT_DIR = ../3rdparty/json_jsonqt/lib

include(../common.pri)

# Library version
VERSION     = $${XIVOVER}
DEFINES     += BASELIB_LIBRARY

INCLUDEPATH += $${COMMONDIR} $${JSON_JSONQT_DIR} src src/gui/

HEADERS     += src/*.h
HEADERS     += src/gui/*.h
HEADERS     += src/dstore/src/*.h
HEADERS     += $${JSON_JSONQT_DIR}/*.h

SOURCES     += src/*.cpp
SOURCES     += src/gui/*.cpp
SOURCES     += src/dstore/src/*.cpp
SOURCES     += $${JSON_JSONQT_DIR}/*.cpp

DESTDIR     = ../bin

QT += network

TRANSLATIONS = baselib_fr.ts
TRANSLATIONS += baselib_nl.ts
TRANSLATIONS += baselib_de.ts

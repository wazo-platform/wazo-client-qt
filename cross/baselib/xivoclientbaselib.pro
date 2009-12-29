include(../../common.pri)

TEMPLATE = lib
QT += network
CONFIG += dll

TARGET = 

DESTDIR     = ../bin

BASELIB_DIR = ../../baselib
COMMON_DIR = ../../common
JSON_JSONQT_DIR = ../../3rdparty/json_jsonqt/lib

# Library version
VERSION     = $${XIVOVER}

INCLUDEPATH += $${COMMON_DIR} $${JSON_JSONQT_DIR} $${BASELIB_DIR}

HEADERS     += $${BASELIB_DIR}/*.h
HEADERS     += $${JSON_JSONQT_DIR}/*.h

SOURCES     += $${BASELIB_DIR}/*.cpp
SOURCES     += $${JSON_JSONQT_DIR}/*.cpp

DEFINES     += BASELIB_LIBRARY

TRANSLATIONS = $${BASELIB_DIR}/baselib_fr.ts

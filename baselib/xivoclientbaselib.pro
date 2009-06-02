# $Id$
TEMPLATE    = lib
COMMONDIR = ../common
JSON_JSONQT_DIR = ../3rdparty/json_jsonqt/lib

include(../common.pri)

VERSION     = 1.1

INCLUDEPATH += $${COMMONDIR} $${JSON_JSONQT_DIR}
HEADERS     = xlet.h   baseengine.h
HEADERS     += phoneinfo.h   userinfo.h   queueinfo.h
HEADERS     += xivoconsts.h
HEADERS     += $${JSON_JSONQT_DIR}/*.h
SOURCES     = xlet.cpp baseengine.cpp
SOURCES     += phoneinfo.cpp userinfo.cpp queueinfo.cpp
SOURCES     += xivoconsts.cpp
SOURCES     += $${JSON_JSONQT_DIR}/*.cpp

DESTDIR     = ../bin

QT += network


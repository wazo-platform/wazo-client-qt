# $Id$
TEMPLATE    = lib
CONFIG      += dll
COMMONDIR = ../common
JSON_JSONQT_DIR = ../3rdparty/json_jsonqt/lib

QMAKE_EXTRA_COMPILERS += updateqm
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
PRE_TARGETDEPS += compiler_updateqm_make_all

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

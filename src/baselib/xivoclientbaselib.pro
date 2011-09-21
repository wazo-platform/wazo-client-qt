include(../common.pri)

TEMPLATE    = lib
CONFIG      += dll

JSON_JSONQT_DIR = $${ROOT_DIR}/3rdparty/json_jsonqt/lib

# Library version
VERSION     = $${XIVOVER}
DEFINES     += BASELIB_LIBRARY

INCLUDEPATH += $${JSON_JSONQT_DIR} dstore

HEADERS     += *.h
HEADERS     += dstore/*.h
HEADERS     += $${JSON_JSONQT_DIR}/*.h

SOURCES     += *.cpp
SOURCES     += dstore/*.cpp
SOURCES     += $${JSON_JSONQT_DIR}/*.cpp

DESTDIR     = $${ROOT_DIR}/bin

QT += network

TRANSLATIONS = $$ROOT_DIR/i18n/baselib_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/baselib_nl.ts

RESOURCES += xivoclientbaselib.qrc

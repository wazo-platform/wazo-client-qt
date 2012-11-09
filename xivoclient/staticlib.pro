include(common-xivoclient.pri)

TEMPLATE = lib
CONFIG   += staticlib hide_symbols

# Library version
VERSION = $${XIVOVER}
DEFINES += BASELIB_LIBRARY
DEFINES += XLETLIB_LIBRARY

TARGET = xivoclient

QT += gui network

JSON_JSONQT_DIR = $${BASELIB_DIR}/json_jsonqt/lib

INCLUDEPATH += $${JSON_JSONQT_DIR}
INCLUDEPATH += $${BASELIB_DIR}/src
INCLUDEPATH += $${BASELIB_DIR}/src/storage

HEADERS     += $${BASELIB_DIR}/src/*.h
HEADERS     += $${BASELIB_DIR}/src/storage/*.h
HEADERS     += $${BASELIB_DIR}/src/dao/*.h
HEADERS     += $${JSON_JSONQT_DIR}/*.h

SOURCES     += $${BASELIB_DIR}/src/*.cpp
SOURCES     += $${BASELIB_DIR}/src/storage/*.cpp
SOURCES     += $${BASELIB_DIR}/src/dao/*.cpp
SOURCES     += $${JSON_JSONQT_DIR}/*.cpp

INCLUDEPATH += $${BASELIB_DIR}/src
INCLUDEPATH += $${BASELIB_DIR}/src/storage
INCLUDEPATH += $${BASELIB_DIR}/src/dao

HEADERS += $${ROOT_DIR}/src/xletlib/*.h
SOURCES += $${ROOT_DIR}/src/xletlib/*.cpp

DESTDIR     = $${BIN_DIR}

RESOURCES += $${BASELIB_DIR}/baselib.qrc xletlib.qrc

MOC_DIR = $$ROOT_DIR/obj
OBJECTS_DIR = $$ROOT_DIR/obj
RCC_DIR = $$ROOT_DIR/obj

TRANSLATIONS += $${BASELIB_DIR}/baselib_en.ts
TRANSLATIONS += $${BASELIB_DIR}/baselib_es_ES.ts
TRANSLATIONS += $${BASELIB_DIR}/baselib_pt_BR.ts
TRANSLATIONS += $${BASELIB_DIR}/baselib_hu.ts
TRANSLATIONS += $${BASELIB_DIR}/baselib_ja.ts
TRANSLATIONS += $${BASELIB_DIR}/baselib_de.ts
TRANSLATIONS += $${BASELIB_DIR}/baselib_it.ts
TRANSLATIONS += $${BASELIB_DIR}/baselib_fr.ts
TRANSLATIONS += $${BASELIB_DIR}/baselib_nl.ts

TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_nl.ts

QMAKE_CXXFLAGS += -g -pg
QMAKE_LFLAGS += -g -pg

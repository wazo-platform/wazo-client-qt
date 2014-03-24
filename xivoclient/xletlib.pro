include(common-no-tests.pri)

TEMPLATE = lib
CONFIG   += dll hide_symbols qt

# Library version
VERSION = $${XIVOVER}
DEFINES += XLETLIB_LIBRARY

TARGET      = xivoclientxlets

INCLUDEPATH += $${BASELIB_DIR}/src
INCLUDEPATH += $${ROOT_DIR}/src

HEADERS += $${ROOT_DIR}/src/xletlib/*.h
SOURCES += $${ROOT_DIR}/src/xletlib/*.cpp
HEADERS += $${ROOT_DIR}/src/xletlib/queue_entries/*.h
SOURCES += $${ROOT_DIR}/src/xletlib/queue_entries/*.cpp


DESTDIR = $${BIN_DIR}

QT += core gui widgets

TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_nl.ts

RESOURCES += xletlib.qrc

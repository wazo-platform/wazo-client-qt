include(common-xivoclient.pri)

TEMPLATE = lib
CONFIG   += dll hide_symbols

# Library version
VERSION = $${XIVOVER}
DEFINES += XLETLIB_LIBRARY

TARGET      = xivoclientxlets

INCLUDEPATH += $${BASELIB_DIR}/src

HEADERS += $${ROOT_DIR}/src/xletlib/*.h
SOURCES += $${ROOT_DIR}/src/xletlib/*.cpp

DESTDIR = $${BIN_DIR}

QT += gui

TRANSLATIONS = $$ROOT_DIR/i18n/xletlib_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xletlib_nl.ts

RESOURCES += xletlib.qrc

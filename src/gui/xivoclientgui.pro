include(../common.pri)

TEMPLATE = lib
CONFIG   += dll

# Library version
VERSION = $${XIVOVER}
DEFINES += GUI_LIBRARY

INCLUDEPATH += ../baselib

HEADERS += *.h
HEADERS += ../baselib/*.h

SOURCES += *.cpp

DESTDIR = $${ROOT_DIR}/bin

QT += gui

TRANSLATIONS = $$ROOT_DIR/i18n/gui_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/gui_nl.ts

RESOURCES += xivoclientgui.qrc

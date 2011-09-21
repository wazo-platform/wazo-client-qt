include(../plugins-global.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/history_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_nl.ts

TARGET      = $$qtLibraryTarget(historyplugin)

RESOURCES = history.qrc

include(../plugins-global.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/records_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/records_nl.ts

TARGET      = $$qtLibraryTarget(recordsplugin)

RESOURCES = records.qrc

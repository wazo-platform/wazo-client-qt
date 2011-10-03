include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/calls_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/calls_nl.ts

TARGET      = $$qtLibraryTarget(callsplugin)

RESOURCES = calls.qrc

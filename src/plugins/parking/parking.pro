include(../plugins-global.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/parking_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/parking_nl.ts

TARGET      = $$qtLibraryTarget(parkingplugin)

RESOURCES = parking.qrc

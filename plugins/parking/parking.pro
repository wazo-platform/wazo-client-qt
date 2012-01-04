include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = parking_fr.ts
TRANSLATIONS += parking_nl.ts
TRANSLATIONS += parking_de.ts
TRANSLATIONS += parking_uk.ts

TARGET      = $$qtLibraryTarget(parkingplugin)

RESOURCES = res.qrc
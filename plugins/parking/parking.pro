include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = parking_fr.ts
TRANSLATIONS += parking_nl.ts

TARGET      = $$qtLibraryTarget(parkingplugin)

RESOURCES = res.qrc

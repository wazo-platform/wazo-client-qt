include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = parking_fr.ts

TARGET      = $$qtLibraryTarget(parkingplugin)

RESOURCES = res.qrc

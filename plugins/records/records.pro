include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = records_fr.ts
TRANSLATIONS += records_nl.ts

TARGET      = $$qtLibraryTarget(recordsplugin)

RESOURCES = res.qrc

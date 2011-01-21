include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = records_fr.ts

TARGET      = $$qtLibraryTarget(recordsplugin)

RESOURCES = res.qrc

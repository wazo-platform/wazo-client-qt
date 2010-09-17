include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = calls_fr.ts

TARGET      = $$qtLibraryTarget(callsplugin)

RESOURCES = res.qrc

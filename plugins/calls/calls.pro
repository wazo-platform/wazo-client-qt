include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = calls_fr.ts
TRANSLATIONS += calls_nl.ts
TRANSLATIONS += calls_de.ts
TRANSLATIONS += calls_uk.ts

TARGET      = $$qtLibraryTarget(callsplugin)

RESOURCES = res.qrc

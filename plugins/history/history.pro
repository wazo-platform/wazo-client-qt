include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = history_fr.ts
TRANSLATIONS += history_nl.ts
TRANSLATIONS += history_de.ts
TRANSLATIONS += history_uk.ts

TARGET      = $$qtLibraryTarget(historyplugin)

RESOURCES = res.qrc

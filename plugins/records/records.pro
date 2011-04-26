include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = records_fr.ts
TRANSLATIONS += records_nl.ts
TRANSLATIONS += records_de.ts
TRANSLATIONS += records_uk.ts

TARGET      = $$qtLibraryTarget(recordsplugin)

RESOURCES = res.qrc

include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = agents_fr.ts
TRANSLATIONS += agents_nl.ts
TRANSLATIONS += agents_de.ts

TARGET      = $$qtLibraryTarget(agentsplugin)

RESOURCES = res.qrc

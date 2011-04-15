include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = agentsnext_fr.ts
TRANSLATIONS += agentsnext_nl.ts
TRANSLATIONS += agentsnext_de.ts

TARGET      = $$qtLibraryTarget(agentsnextplugin)

RESOURCES = res.qrc

include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = agents_fr.ts

TARGET      = $$qtLibraryTarget(agentsplugin)

RESOURCES = res.qrc

include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = agentsnext_fr.ts

TARGET      = $$qtLibraryTarget(agentsnextplugin)

RESOURCES = res.qrc

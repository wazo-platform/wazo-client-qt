include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = agentdetails_fr.ts
TRANSLATIONS += agentdetails_nl.ts
TRANSLATIONS += agentdetails_de.ts

TARGET      = $$qtLibraryTarget(agentdetailsplugin)

RESOURCES = res.qrc

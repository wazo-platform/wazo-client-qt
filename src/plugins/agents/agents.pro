include(../plugins-global.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/agents_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_nl.ts

TARGET      = $$qtLibraryTarget(agentsplugin)

RESOURCES = agents.qrc

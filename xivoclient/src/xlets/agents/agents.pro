include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/agents_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_nl.ts

TARGET      = agentsplugin

RESOURCES = agents.qrc

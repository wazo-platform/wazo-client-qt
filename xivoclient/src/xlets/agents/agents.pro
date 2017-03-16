include(../../../common-xlets.pri)

QT += widgets

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/agents_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_es_CL.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_nl.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agents_pl.ts

TARGET      = agentsplugin

RESOURCES = agents.qrc

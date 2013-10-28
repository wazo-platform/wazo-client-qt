include(../../../common-xlets.pri)

QT += widgets

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/agentdetails_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentdetails_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentdetails_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentdetails_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentdetails_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentdetails_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentdetails_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentdetails_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentdetails_nl.ts

TARGET      = agentdetailsplugin

RESOURCES = agentdetails.qrc

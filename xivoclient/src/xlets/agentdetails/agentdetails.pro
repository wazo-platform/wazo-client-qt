include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/agentdetails_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentdetails_nl.ts

TARGET      = $$qtLibraryTarget(agentdetailsplugin)

RESOURCES = agentdetails.qrc

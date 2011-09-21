include(../plugins-global.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/agentsnext_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/agentsnext_nl.ts

TARGET      = $$qtLibraryTarget(agentsnextplugin)

RESOURCES = agentsnext.qrc

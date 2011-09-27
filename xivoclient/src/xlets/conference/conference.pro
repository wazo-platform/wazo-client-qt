include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/conference_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/conference_nl.ts

TARGET      = $$qtLibraryTarget(conferenceplugin)

RESOURCES = conference.qrc

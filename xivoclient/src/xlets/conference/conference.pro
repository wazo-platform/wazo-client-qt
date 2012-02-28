include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/conference_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/conference_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/conference_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/conference_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/conference_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/conference_nl.ts

TARGET      = conferenceplugin

RESOURCES = conference.qrc

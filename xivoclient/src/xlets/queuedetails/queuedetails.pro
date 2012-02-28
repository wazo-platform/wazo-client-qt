include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/queuedetails_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queuedetails_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queuedetails_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queuedetails_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queuedetails_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queuedetails_nl.ts

TARGET      = queuedetailsplugin

RESOURCES = queuedetails.qrc

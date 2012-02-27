include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/history_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_nl.ts

TARGET      = historyplugin

RESOURCES = history.qrc

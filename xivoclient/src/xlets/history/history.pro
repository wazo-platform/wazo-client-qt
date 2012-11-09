include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/history_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/history_nl.ts

TARGET      = historyplugin

RESOURCES = history.qrc

include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_es_CL.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_nl.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_entries_pl.ts

TARGET      = queueentrydetailsplugin

RESOURCES = queue_entries.qrc

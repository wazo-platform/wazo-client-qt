include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/queues_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_es_CL.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_nl.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_pl.ts

TARGET      = queuesplugin

RESOURCES = queues.qrc

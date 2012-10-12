include(../../../common-xlets.pri)

HEADERS = *.h
SOURCES = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/queue_members_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_members_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_members_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_members_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_members_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_members_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_members_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_members_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queue_members_nl.ts

TARGET = queuemembersplugin

RESOURCES = queue_members.qrc

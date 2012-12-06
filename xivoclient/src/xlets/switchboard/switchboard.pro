include(../../../common-xlets.pri)
HEADERS = *.h
SOURCES = *.cpp
FORMS = switchboard.ui
FORMS += current_call.ui
TARGET = switchboardplugin

TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_nl.ts

RESOURCES = switchboard.qrc

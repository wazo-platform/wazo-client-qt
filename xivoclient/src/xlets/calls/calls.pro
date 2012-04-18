include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/calls_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/calls_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/calls_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/calls_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/calls_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/calls_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/calls_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/calls_nl.ts

TARGET      = callsplugin

RESOURCES = calls.qrc

include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/services_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/services_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/services_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/services_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/services_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/services_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/services_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/services_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/services_nl.ts

TARGET      = featuresplugin

RESOURCES = services.qrc

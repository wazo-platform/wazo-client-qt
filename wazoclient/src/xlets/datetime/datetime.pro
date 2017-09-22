include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_es_CL.ts
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_nl.ts

TARGET      = datetimeplugin

RESOURCES = datetime.qrc

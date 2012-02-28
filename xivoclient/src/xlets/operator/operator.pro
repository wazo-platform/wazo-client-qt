include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/operator_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/operator_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/operator_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/operator_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/operator_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/operator_nl.ts

TARGET      = operatorplugin

RESOURCES = operator.qrc

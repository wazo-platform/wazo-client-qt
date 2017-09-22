include(../../../common-xlets.pri)

QT += svg

HEADERS     = *.h
SOURCES     = *.cpp
FORMS       = fax_widget.ui

TARGET      = faxplugin

TRANSLATIONS += $$ROOT_DIR/i18n/fax_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/fax_es_CL.ts
TRANSLATIONS += $$ROOT_DIR/i18n/fax_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/fax_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/fax_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/fax_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/fax_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/fax_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/fax_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/fax_nl.ts

RESOURCES = fax.qrc

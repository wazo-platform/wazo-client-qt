include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
FORMS       = directory_widget.ui

TARGET      = directoryplugin

TRANSLATIONS += $$ROOT_DIR/i18n/directory_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/directory_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/directory_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/directory_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/directory_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/directory_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/directory_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/directory_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/directory_nl.ts

RESOURCES = directory.qrc

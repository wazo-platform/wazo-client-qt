include(../../../common-xlets.pri)

QT += svg

HEADERS     = *.h
SOURCES     = *.cpp
FORMS       = identity.ui

TARGET      = identityplugin

TRANSLATIONS += $$ROOT_DIR/i18n/identity_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_es_CL.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_nl.ts

RESOURCES = identity.qrc

include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/identity_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_nl.ts

TARGET      = identityplugin

RESOURCES = identity.qrc

include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/identity_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_nl.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_ja.ts

TARGET      = $$qtLibraryTarget(identityplugin)

RESOURCES = identity.qrc

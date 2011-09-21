include(../plugins-global.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/identity_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/identity_nl.ts

TARGET      = $$qtLibraryTarget(identityplugin)

RESOURCES = identity.qrc

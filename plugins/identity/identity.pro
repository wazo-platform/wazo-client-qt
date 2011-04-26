include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = identity_fr.ts
TRANSLATIONS += identity_nl.ts
TRANSLATIONS += identity_de.ts
TRANSLATIONS += identity_uk.ts

TARGET      = $$qtLibraryTarget(identityplugin)

RESOURCES = res.qrc

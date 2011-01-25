include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = identity_fr.ts
TRANSLATIONS += identity_nl.ts

TARGET      = $$qtLibraryTarget(identityplugin)

RESOURCES = res.qrc

include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = identity_fr.ts

TARGET      = $$qtLibraryTarget(identityplugin)

RESOURCES = res.qrc

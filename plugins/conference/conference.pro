include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = conference_fr.ts
TRANSLATIONS += conference_nl.ts

TARGET      = $$qtLibraryTarget(conferenceplugin)

RESOURCES = res.qrc

include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = conference_fr.ts

TARGET      = $$qtLibraryTarget(conferenceplugin)

RESOURCES = res.qrc

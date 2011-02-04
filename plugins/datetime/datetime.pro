include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = datetime_fr.ts
TRANSLATIONS = datetime_nl.ts

TARGET      = $$qtLibraryTarget(datetimeplugin)

RESOURCES = res.qrc

include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = datetime_fr.ts

TARGET      = $$qtLibraryTarget(datetimeplugin)

RESOURCES = res.qrc

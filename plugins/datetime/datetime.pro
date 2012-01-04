include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = datetime_fr.ts
TRANSLATIONS += datetime_nl.ts
TRANSLATIONS += datetime_de.ts
TRANSLATIONS += datetime_uk.ts

TARGET      = $$qtLibraryTarget(datetimeplugin)

RESOURCES = res.qrc
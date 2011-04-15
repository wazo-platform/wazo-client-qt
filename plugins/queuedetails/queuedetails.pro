include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = queuedetails_fr.ts
TRANSLATIONS += queuedetails_nl.ts
TRANSLATIONS += queuedetails_de.ts

TARGET      = $$qtLibraryTarget(queuedetailsplugin)

RESOURCES = res.qrc

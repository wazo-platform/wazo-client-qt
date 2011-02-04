include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = queueentrydetails_fr.ts
TRANSLATIONS += queueentrydetails_nl.ts

TARGET      = $$qtLibraryTarget(queueentrydetailsplugin)

RESOURCES = res.qrc

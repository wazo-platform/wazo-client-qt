include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = queueentrydetails_fr.ts

TARGET      = $$qtLibraryTarget(queueentrydetailsplugin)

RESOURCES = res.qrc

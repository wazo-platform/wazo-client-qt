include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = queuedetails_fr.ts

TARGET      = $$qtLibraryTarget(queuedetailsplugin)

RESOURCES = res.qrc

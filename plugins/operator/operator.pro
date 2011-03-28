include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = operator_fr.ts
TRANSLATIONS += operator_nl.ts

TARGET      = $$qtLibraryTarget(operatorplugin)

RESOURCES = res.qrc

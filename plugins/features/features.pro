include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = features_fr.ts

TARGET      = $$qtLibraryTarget(featuresplugin)

RESOURCES = res.qrc

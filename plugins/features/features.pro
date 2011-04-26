include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = features_fr.ts
TRANSLATIONS += features_nl.ts
TRANSLATIONS += features_de.ts
TRANSLATIONS += features_uk.ts

TARGET      = $$qtLibraryTarget(featuresplugin)

RESOURCES = res.qrc

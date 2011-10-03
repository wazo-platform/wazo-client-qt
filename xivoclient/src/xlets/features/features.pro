include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/features_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/features_nl.ts

TARGET      = $$qtLibraryTarget(featuresplugin)

RESOURCES = features.qrc

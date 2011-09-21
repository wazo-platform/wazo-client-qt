include(../plugins-global.pri)

HEADERS     = *.h
SOURCES     = *.cpp
QT          += xml
TRANSLATIONS = $$ROOT_DIR/i18n/popcaastra_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/popcaastra_nl.ts

TARGET      = $$qtLibraryTarget(popcaastraplugin)

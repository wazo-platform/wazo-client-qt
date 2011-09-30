include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
QT          += xml
TRANSLATIONS = popcaastra_fr.ts

TARGET      = $$qtLibraryTarget(popcaastraplugin)

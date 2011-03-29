include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
FORMS       += src/popcaastra.ui
QT          += xml

TARGET      = $$qtLibraryTarget(popcaastraplugin)

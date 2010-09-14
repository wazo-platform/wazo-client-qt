include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp

include(../../qtaddons/qtcolorpicker/src/qtcolorpicker.pri)

TRANSLATIONS = switchboard_fr.ts

TARGET      = $$qtLibraryTarget(switchboardplugin)

RESOURCES = res.qrc

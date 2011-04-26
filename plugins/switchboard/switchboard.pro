include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = switchboard_fr.ts
TRANSLATIONS += switchboard_nl.ts
TRANSLATIONS += switchboard_de.ts
TRANSLATIONS += switchboard_uk.ts

include(../../qtaddons/qtcolorpicker/src/qtcolorpicker.pri)

TARGET      = $$qtLibraryTarget(switchboardplugin)

RESOURCES = res.qrc

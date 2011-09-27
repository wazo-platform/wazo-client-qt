include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/switchboard_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_nl.ts

include($${ROOT_DIR}/qtaddons/qtcolorpicker/src/qtcolorpicker.pri)

TARGET      = $$qtLibraryTarget(switchboardplugin)

RESOURCES = switchboard.qrc

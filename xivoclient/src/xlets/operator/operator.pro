include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/operator_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/operator_nl.ts

TARGET      = $$qtLibraryTarget(operatorplugin)

RESOURCES = operator.qrc

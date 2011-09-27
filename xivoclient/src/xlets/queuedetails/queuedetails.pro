include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/queuedetails_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queuedetails_nl.ts

TARGET      = $$qtLibraryTarget(queuedetailsplugin)

RESOURCES = queuedetails.qrc

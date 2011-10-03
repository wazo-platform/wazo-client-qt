include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/queues_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/queues_nl.ts

TARGET      = $$qtLibraryTarget(queuesplugin)

RESOURCES = queues.qrc

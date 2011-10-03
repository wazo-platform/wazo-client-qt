include(../../../common-xlets.pri)

HEADERS     = outlook*.h
SOURCES     = outlook*.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/outlook_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/outlook_nl.ts

TARGET      = $$qtLibraryTarget(outlookplugin)

win32:LIBS  += -luuid -lole32 -loleaut32

QT += network sql
RESOURCES = outlook.qrc

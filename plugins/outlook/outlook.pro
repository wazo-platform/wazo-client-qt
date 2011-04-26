include(../plugins-global.pri)

HEADERS     = outlook*.h
SOURCES     = outlook*.cpp
TRANSLATIONS = outlook_fr.ts
TRANSLATIONS += outlook_nl.ts
TRANSLATIONS += outlook_de.ts
TRANSLATIONS += outlook_uk.ts

TARGET      = $$qtLibraryTarget(outlookplugin)

win32:LIBS  += -luuid -lole32 -loleaut32

QT += network sql
RESOURCES = res.qrc

include(../plugins-global.pri)

HEADERS     = outlook*.h
SOURCES     = outlook*.cpp
TRANSLATIONS = outlook_fr.ts

TARGET      = $$qtLibraryTarget(outlookplugin)

win32:LIBS  += -luuid -lole32 -loleaut32

QT += network sql
RESOURCES = res.qrc

include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = queues_fr.ts
TRANSLATIONS += queues_nl.ts

TARGET      = $$qtLibraryTarget(queuesplugin)

RESOURCES = res.qrc

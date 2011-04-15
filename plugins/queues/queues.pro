include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = queues_fr.ts
TRANSLATIONS += queues_nl.ts
TRANSLATIONS += queues_de.ts

TARGET      = $$qtLibraryTarget(queuesplugin)

RESOURCES = res.qrc

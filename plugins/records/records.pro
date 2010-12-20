include(../plugins-global.pri)

HEADERS     = src/*.h
SOURCES     = src/*.cpp
TRANSLATIONS = records_*.ts

TARGET      = $$qtLibraryTarget(recordsplugin)

RESOURCES = res.qrc

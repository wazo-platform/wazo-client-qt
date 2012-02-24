include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/datetime_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/datetime_nl.ts

TARGET      = datetimeplugin

RESOURCES = datetime.qrc

include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS = $$ROOT_DIR/i18n/services_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/services_nl.ts

TARGET      = featuresplugin

RESOURCES = services.qrc

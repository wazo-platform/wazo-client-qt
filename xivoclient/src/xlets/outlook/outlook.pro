include(../../../common-xlets.pri)

HEADERS     = outlook*.h
SOURCES     = outlook*.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/outlook_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/outlook_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/outlook_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/outlook_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/outlook_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/outlook_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/outlook_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/outlook_nl.ts

TARGET      = outlookplugin

win32:LIBS  += -luuid -lole32 -loleaut32

QT += network sql
RESOURCES = outlook.qrc

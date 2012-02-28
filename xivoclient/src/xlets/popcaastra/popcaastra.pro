include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
QT          += xml
TRANSLATIONS += $$ROOT_DIR/i18n/popcaastra_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/popcaastra_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/popcaastra_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/popcaastra_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/popcaastra_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/popcaastra_nl.ts

TARGET      = popcaastraplugin

include(../../../common-xlets.pri)

HEADERS     = *.h
SOURCES     = *.cpp
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_it.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/switchboard_nl.ts

include($${ROOT_DIR}/qtaddons/qtcolorpicker/src/qtcolorpicker.pri)

TARGET      = switchboardplugin

RESOURCES = switchboard.qrc

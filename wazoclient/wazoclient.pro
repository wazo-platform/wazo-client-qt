include(common-no-tests.pri)
include($${ROOT_DIR}/qt-solutions/qtsingleapplication/src/qtsingleapplication.pri)

TEMPLATE = app
# CONFIG += console # uncomment to get console on Win32

TARGET = wazoclient

unix:LIBS += -lwazoclientxlets
win32 {
    debug:LIBS += -lwazoclientxletsd
    release:LIBS += -lwazoclientxlets
}
mac {
    QMAKE_INFO_PLIST = ../packaging/resources/macos-info.plist
}

QT += svg xml

RESOURCES += wazoclient.qrc
mac {
    ICON = ../packaging/resources/wazoicon-mac.icns
} else {
    RC_FILE = wazoclient.rc
}

FORMS += $${ROOT_DIR}/src/config_widget/config_widget.ui
FORMS += $${ROOT_DIR}/src/login_widget/login_widget.ui
FORMS += $${ROOT_DIR}/src/main_window/loading_dialog.ui
FORMS += $${ROOT_DIR}/src/main_window/main_window.ui

# this should just tell lrelease to compile these files
TRANSLATIONS += $$ROOT_DIR/i18n/wazoclient_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/wazoclient_nl.ts
TRANSLATIONS += $$ROOT_DIR/i18n/wazoclient_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/wazoclient_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/wazoclient_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/wazoclient_es_CL.ts
TRANSLATIONS += $$ROOT_DIR/i18n/wazoclient_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/wazoclient_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/wazoclient_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/wazoclient_it.ts

# Input
INCLUDEPATH += $$BASELIB_DIR/src
INCLUDEPATH += $${ROOT_DIR}/src

HEADERS += $${ROOT_DIR}/src/*.h
SOURCES += $${ROOT_DIR}/src/*.cpp
HEADERS += $${ROOT_DIR}/src/config_widget/*.h
SOURCES += $${ROOT_DIR}/src/config_widget/*.cpp
HEADERS += $${ROOT_DIR}/src/login_widget/*.h
SOURCES += $${ROOT_DIR}/src/login_widget/*.cpp
HEADERS += $${ROOT_DIR}/src/main_window/*.h
SOURCES += $${ROOT_DIR}/src/main_window/*.cpp
HEADERS += $${ROOT_DIR}/src/remote_control/*.h
SOURCES += $${ROOT_DIR}/src/remote_control/*.cpp

HEADERS += $${ROOT_DIR}/src/xlets/customerinfo-builtin/*.h
SOURCES += $${ROOT_DIR}/src/xlets/customerinfo-builtin/*.cpp

DESTDIR  = $$BIN_DIR

# Get the optional plugins dir from environment variable
PLUGINDIR = $$(XIVOCLIENT_PLUGINDIR)
isEmpty( PLUGINDIR ) {
    PLUGINDIR = /usr/share/wazoclient/plugins
}
DEFINES += PLUGINDIR=\"\\\"$${PLUGINDIR}\\\"\"

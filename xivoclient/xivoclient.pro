include(common-no-tests.pri)
include($${ROOT_DIR}/qtaddons/qtsingleapplication/src/qtsingleapplication.pri)

TEMPLATE = app
# CONFIG += console # uncomment to get console on Win32

TARGET = xivoclient

CONFIG += uitools

!profiling {
    unix:LIBS += -lxivoclientxlets
    win32 {
        debug:LIBS += -lxivoclientxletsd
        release:LIBS += -lxivoclientxlets
    }
}
mac {
    QMAKE_INFO_PLIST = ../packaging/resources/macos-info.plist
}

QT += xml

RESOURCES += xivoclient.qrc
mac {
    ICON = ../packaging/resources/xivoicon-mac.icns
} else {
    RC_FILE = xivoclient.rc
}

FORMS += $${ROOT_DIR}/src/config_widget/config_widget.ui

# this should just tell lrelease to compile these files
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_nl.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_ja.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_en.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_es_ES.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_pt_BR.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_hu.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_it.ts

# Input
INCLUDEPATH += $$BASELIB_DIR/src
INCLUDEPATH += $${BASELIB_DIR}/json_jsonqt/lib
INCLUDEPATH += $${ROOT_DIR}/src

HEADERS += $${ROOT_DIR}/src/*.h
SOURCES += $${ROOT_DIR}/src/*.cpp
HEADERS += $${ROOT_DIR}/src/config_widget/*.h
SOURCES += $${ROOT_DIR}/src/config_widget/*.cpp
HEADERS += $${ROOT_DIR}/src/remote_control/*.h
SOURCES += $${ROOT_DIR}/src/remote_control/*.cpp

BUILTIN_DIRS += customerinfo-builtin
BUILTIN_DIRS += dial-builtin
BUILTIN_DIRS += remotedirectory-builtin
BUILTIN_DIRS += fax-builtin
BUILTIN_DIRS += mylocaldir-builtin
BUILTIN_DIRS += search-builtin
for(BUILTIN_DIR, BUILTIN_DIRS){
    HEADERS     += $${ROOT_DIR}/src/xlets/$${BUILTIN_DIR}/*.h
    # SOURCES     += $${ROOT_DIR}/src/xlets/$${BUILTIN_DIR}/*.cpp
}

# necessary to make lupdate process these source files
SOURCES += $${ROOT_DIR}/src/xlets/customerinfo-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/dial-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/remotedirectory-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/fax-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/mylocaldir-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/search-builtin/*.cpp

DESTDIR  = $$BIN_DIR

# Get the optional plugins dir from environment variable
PLUGINDIR = $$(XIVOCLIENT_PLUGINDIR)
isEmpty( PLUGINDIR ) {
    PLUGINDIR = /usr/share/xivoclient/plugins
}
DEFINES += PLUGINDIR=\"\\\"$${PLUGINDIR}\\\"\"

profiling {
    QMAKE_CXXFLAGS += -g -pg
    QMAKE_LFLAGS += -g -pg
}

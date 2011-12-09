include(common-xivoclient.pri)
include($${ROOT_DIR}/qtaddons/qtsingleapplication/src/qtsingleapplication.pri)

TEMPLATE = app
# CONFIG += console # uncomment to get console on Win32
CONFIG += uitools

unix:LIBS += -lxivoclientxlets
win32 {
    debug:LIBS += -lxivoclientxletsd1
    release:LIBS += -lxivoclientxlets1
}

QT += xml

RESOURCES += xivoclient.qrc
RC_FILE = xivoclient.rc

# this should just tell lrelease to compile these files
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_nl.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_de.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_ja.ts

# Input
INCLUDEPATH += $$BASELIB_DIR/src $${ROOT_DIR}/src/xletlib
HEADERS += $${ROOT_DIR}/src/*.h
SOURCES += $${ROOT_DIR}/src/*.cpp

BUILTIN_DIRS += callcampaign-builtin
BUILTIN_DIRS += customerinfo-builtin
BUILTIN_DIRS += dial-builtin
BUILTIN_DIRS += directory-builtin
BUILTIN_DIRS += fax-builtin
BUILTIN_DIRS += mylocaldir-builtin
BUILTIN_DIRS += search-builtin
for(BUILTIN_DIR, BUILTIN_DIRS){
    INCLUDEPATH += $${ROOT_DIR}/src/xlets/$${BUILTIN_DIR}
    HEADERS     += $${ROOT_DIR}/src/xlets/$${BUILTIN_DIR}/*.h
    # SOURCES     += $${ROOT_DIR}/src/xlets/$${BUILTIN_DIR}/*.cpp
}

# necessary to make lupdate process these source files
SOURCES += $${ROOT_DIR}/src/xlets/callcampaign-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/customerinfo-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/dial-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/directory-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/fax-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/mylocaldir-builtin/*.cpp
SOURCES += $${ROOT_DIR}/src/xlets/search-builtin/*.cpp

DESTDIR  = $$BIN_DIR

# Get the optional plugins dir from shell env
PLUGINDIR = $$system(echo -n $XIVOCLIENT_PLUGINDIR)
isEmpty( PLUGINDIR ) {
    PLUGINDIR = /usr/share/xivoclient/plugins
}
DEFINES += PLUGINDIR=\"\\\"$${PLUGINDIR}\\\"\"

include(common-xivoclient.pri)
include($${ROOT_DIR}/qtaddons/qtsingleapplication/src/qtsingleapplication.pri)

TEMPLATE = app
# CONFIG += console # uncomment to get console on Win32
CONFIG += uitools
INCLUDEPATH += $$BASELIB_DIR/src $${ROOT_DIR}/src/xletlib

# Input
HEADERS += $${ROOT_DIR}/src/*.h
SOURCES += $${ROOT_DIR}/src/*.cpp
XC_BUILTIN = callcampaign-builtin customerinfo-builtin dial-builtin directory-builtin fax-builtin mylocaldir-builtin search-builtin
for(xletdir, XC_BUILTIN){
    INCLUDEPATH += $${ROOT_DIR}/src/xlets/$${xletdir}
    HEADERS     += $${ROOT_DIR}/src/xlets/$${xletdir}/*.h
    # SOURCES     += $${ROOT_DIR}/src/xlets/$${xletdir}/*.cpp
}

LIBS += -L$${BASELIB_DIR}/bin -L$${BIN_DIR}
unix:LIBS += -lxivoclient -lxivoclientxlets
win32:LIBS += -lxivoclient1 -lxivoclientxlets1

win32 {
    LIBS += -lole32 -loleaut32 -luuid
}

QT += xml

RESOURCES += xivoclient.qrc
RC_FILE = xivoclient.rc

# this should just tell lrelease to compile these files
TRANSLATIONS  = $$ROOT_DIR/i18n/xivoclient_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_nl.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_de.ts

# necessary to make lupdate process these source files
SOURCES      += $${ROOT_DIR}/src/xlets/callcampaign-builtin/*.cpp
SOURCES      += $${ROOT_DIR}/src/xlets/customerinfo-builtin/*.cpp
SOURCES      += $${ROOT_DIR}/src/xlets/dial-builtin/*.cpp
SOURCES      += $${ROOT_DIR}/src/xlets/directory-builtin/*.cpp
SOURCES      += $${ROOT_DIR}/src/xlets/fax-builtin/*.cpp
SOURCES      += $${ROOT_DIR}/src/xlets/mylocaldir-builtin/*.cpp
SOURCES      += $${ROOT_DIR}/src/xlets/search-builtin/*.cpp

DESTDIR  = $$BIN_DIR

# Get the optional plugins dir from shell env
PLUGINDIR = $$system(echo -n $XIVOCLIENT_PLUGINDIR)
isEmpty( PLUGINDIR ) {
    PLUGINDIR = /usr/share/xivoclient/plugins
}
DEFINES += PLUGINDIR=\"\\\"$${PLUGINDIR}\\\"\"

MAKEFILE = Makefile_xivoclient

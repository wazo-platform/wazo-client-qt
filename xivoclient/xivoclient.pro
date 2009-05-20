######################################################################
#
# $Revision$
# $Date$
#

include(../qtaddons/qtsingleapplication/src/qtsingleapplication.pri)

COMMONDIR = ../common
JSON_JSONQT_DIR = ../3rdparty/json_jsonqt/lib
OUTLOOK_DIR = ../3rdparty/outlook

_XIVOVER_ = 1.1
_SVNVER_ = 1
include(versions.pro)
XIVOVER = '\\"$${_XIVOVER_}\\"'
SVNVER  = '\\"$${_SVNVER_}\\"'
DATEBUILD = '\\"$${_DATEBUILD_}\\"'
message('XIVO version:' $${XIVOVER})
message(' svn version:' $${SVNVER})
message('  build date:' $${DATEBUILD})
DEFINES += XIVOVER=\"$${XIVOVER}\"
DEFINES += SVNVER=\"$${SVNVER}\"
DEFINES += DATEBUILD=\"$${DATEBUILD}\"

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . $${COMMONDIR} $${JSON_JSONQT_DIR}

CONFIG -= debug
# CONFIG += console # uncomment to get console on Win32
CONFIG += static
CONFIG += uitools
CONFIG += x86 ppc

# Input
HEADERS += mainwidget.h powerawareapplication.h $${COMMONDIR}/*.h
HEADERS += $${JSON_JSONQT_DIR}/*.h

SOURCES += main.cpp mainwidget.cpp powerawareapplication.cpp $${COMMONDIR}/*.cpp
SOURCES += $${JSON_JSONQT_DIR}/*.cpp

win32 {
	INCLUDEPATH += $${OUTLOOK_DIR}
        DEFINES += USE_OUTLOOK=1 SQLITE_THREADSAFE=1
        LIBS += -lole32 -loleaut32 -luuid
        HEADERS += $${OUTLOOK_DIR}/*.h
        SOURCES += $${OUTLOOK_DIR}/*.cpp
        HEADERS += $${OUTLOOK_DIR}/sqlite3/*.h
        SOURCES += $${OUTLOOK_DIR}/sqlite3/*.c
}

QT += network
QT += xml

# let's forget it for the time being (2008/11/10) :
# - makes xivoclient 60% more heavier (static mode)
# - recompilation under win32 is not straightforward yet
# - no hurry for actual use
exists(../webkit.mak) {
        QT += webkit
        DEFINES += USE_WEBKIT
}

RESOURCES += appli.qrc
TRANSLATIONS = xivoclient_fr.ts qt_fr.ts
# TRANSLATIONS = xivoclient_fr.ts $$[QT_INSTALL_PREFIX]/translations/qt_fr.ts
RC_FILE = appli.rc

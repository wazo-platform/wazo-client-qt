######################################################################
#
# $Revision$
# $Date$
#

COMMONDIR = ../common
# JSON_MJSON_DIR = ../json_mjson
# JSON_JAULA_DIR = ../json_jaula

SVNVER_  = $$system(touch $${COMMONDIR}/xivoconsts.h mainwidget.cpp && svn info .. | tail -3 | head -1 | sed "s/.*:.//")
SVNVER   = '\\"$${SVNVER_}\\"'          # place quotes around the version string
XIVOVER_ = $$system(cat ../../VERSION)
XIVOVER  = '\\"$${XIVOVER_}\\"'         # place quotes around the version string

DEFINES += SVNVER=\"$${SVNVER}\"    # create a VER macro containing the version string
DEFINES += XIVOVER=\"$${XIVOVER}\"

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . $${COMMONDIR}
CONFIG -= debug
CONFIG += static
CONFIG += uitools
CONFIG += x86 ppc

# Input
HEADERS += mainwidget.h $${COMMONDIR}/*.h
# HEADERS += $${JSON_MJSON_DIR}/json.h

SOURCES += main.cpp mainwidget.cpp $${COMMONDIR}/*.cpp
# SOURCES += $${JSON_MJSON_DIR}/json.c

QT += network
QT += xml
RESOURCES += appli.qrc
TRANSLATIONS = xivoclient_fr.ts qt_fr.ts
# TRANSLATIONS = xivoclient_fr.ts $$[QT_INSTALL_PREFIX]/translations/qt_fr.ts
RC_FILE = appli.rc

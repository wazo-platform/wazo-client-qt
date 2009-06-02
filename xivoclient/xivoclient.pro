######################################################################
#
# $Revision$
# $Date$
#

include(../qtaddons/qtsingleapplication/src/qtsingleapplication.pri)
include(../qtaddons/qtcolorpicker/src/qtcolorpicker.pri)

COMMONDIR = ../common
OUTLOOK_DIR = ../3rdparty/outlook

include(../common.pri)

TEMPLATE = app
# CONFIG += console # uncomment to get console on Win32
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . $${COMMONDIR} ../baselib

# Input
HEADERS += mainwidget.h powerawareapplication.h
HEADERS += $${COMMONDIR}/*.h

SOURCES += main.cpp mainwidget.cpp powerawareapplication.cpp
SOURCES += $${COMMONDIR}/*.cpp

unix:LIBS += ../bin/libxivoclientbaselib.so
# win32:LIBS += ../bin/libxivoclientbaselib1.a
win32:LIBS += -L"..\\bin" -lxivoclientbaselib1

win32 {
	INCLUDEPATH += $${OUTLOOK_DIR}
        DEFINES += USE_OUTLOOK=1 SQLITE_THREADSAFE=1
        LIBS += -lole32 -loleaut32 -luuid
        HEADERS += $${OUTLOOK_DIR}/*.h
        SOURCES += $${OUTLOOK_DIR}/*.cpp
        HEADERS += $${OUTLOOK_DIR}/sqlite3/*.h
        SOURCES += $${OUTLOOK_DIR}/sqlite3/*.c
}

QT += xml

RESOURCES += appli.qrc
TRANSLATIONS = xivoclient_fr.ts qt_fr.ts
# TRANSLATIONS = xivoclient_fr.ts $$[QT_INSTALL_PREFIX]/translations/qt_fr.ts
RC_FILE = appli.rc

DESTDIR  = ../bin


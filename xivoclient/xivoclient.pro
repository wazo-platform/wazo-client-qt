######################################################################
#
# $Revision$
# $Date$
#

include(../qtaddons/qtsingleapplication/src/qtsingleapplication.pri)
include(../qtaddons/qtcolorpicker/src/qtcolorpicker.pri)

COMMONDIR = ../common

include(../common.pri)

TEMPLATE = app
# CONFIG += console # uncomment to get console on Win32
TARGET = 
CONFIG += uitools
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
    LIBS += -lole32 -loleaut32 -luuid
}

QT += xml

RESOURCES += appli.qrc
TRANSLATIONS = xivoclient_fr.ts qt_fr.ts
# TRANSLATIONS = xivoclient_fr.ts $$[QT_INSTALL_PREFIX]/translations/qt_fr.ts
RC_FILE = appli.rc

DESTDIR  = ../bin


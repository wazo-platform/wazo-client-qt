######################################################################
#
# $Revision$
# $Date$
#

include(../qtaddons/qtsingleapplication/src/qtsingleapplication.pri)

COMMONDIR = ../common

include(../common.pri)

TEMPLATE = app
# CONFIG += console # uncomment to get console on Win32
TARGET = 
CONFIG += uitools
DEPENDPATH += .
INCLUDEPATH += . $${COMMONDIR} $${COMMONDIR}/built-in-xlet ../baselib/src ../baselib/src/gui

# Input
HEADERS += src/*.h $${COMMONDIR}/*.h $${COMMONDIR}/built-in-xlet/*.h

SOURCES += src/*.cpp $${COMMONDIR}/*.cpp $${COMMONDIR}/built-in-xlet/*.cpp

unix:LIBS += -L../bin -lxivoclientbaselib
win32:LIBS += -L../bin -lxivoclientbaselib1

win32 {
    LIBS += -lole32 -loleaut32 -luuid
}

QT += xml

RESOURCES += appli.qrc

# this should just tell lrelease to compile these files
TRANSLATIONS = xivoclient_fr.ts

RC_FILE = appli.rc

DESTDIR  = ../bin

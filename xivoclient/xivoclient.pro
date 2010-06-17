######################################################################
#
# $Revision$
# $Date$
#

include(../qtaddons/qtsingleapplication/src/qtsingleapplication.pri)
include(../qtaddons/qtcolorpicker/src/qtcolorpicker.pri)

COMMONDIR = ../common

QMAKE_EXTRA_COMPILERS += updateqm
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
PRE_TARGETDEPS += compiler_updateqm_make_all

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

unix:LIBS += -L../bin/ -lxivoclientbaselib
# win32:LIBS += ../bin/libxivoclientbaselib1.a
win32:LIBS += -L"..\\bin" -lxivoclientbaselib1

win32 {
    LIBS += -lole32 -loleaut32 -luuid
}

QT += xml

RESOURCES += appli.qrc

# this should just tell lrelease to compile these files
TRANSLATIONS = xivoclient_fr.ts qt_fr.ts
# TRANSLATIONS = xivoclient_fr.ts $$[QT_INSTALL_PREFIX]/translations/qt_fr.ts

RC_FILE = appli.rc

DESTDIR  = ../bin

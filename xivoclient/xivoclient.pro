######################################################################
#
# $Revision: 9722 $
# $Date: 2010-12-09 10:53:51 +0100 (Thu, 09 Dec 2010) $
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
TRANSLATIONS += xivoclient_nl.ts
TRANSLATIONS += xivoclient_de.ts

RC_FILE = appli.rc

DESTDIR  = ../bin

# Get the plugins dir from shell env
PLUGINDIR = $$system(echo -n $XIVOCLIENT_PLUGINDIR)
isEmpty( PLUGINDIR ) {
    PLUGINDIR = /usr/share/xivoclient/plugins
}
DEFINES += PLUGINDIR=\"\\\"$${PLUGINDIR}\\\"\"

# GNU/Linux = strip and pack. Avoids stripping an already packed version ...
# These commands are executed just after the final compilation of the executable.
unix:CONFIG(release,debug|release) {
    QMAKE_POST_LINK += strip $(TARGET) ;
    QMAKE_POST_LINK += upx $(TARGET) ;
}

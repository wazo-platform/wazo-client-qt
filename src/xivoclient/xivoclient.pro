include(../common.pri)
include($$ROOT_DIR/qtaddons/qtsingleapplication/src/qtsingleapplication.pri)

TEMPLATE = app
# CONFIG += console # uncomment to get console on Win32
TARGET =
CONFIG += uitools
DEPENDPATH += .
INCLUDEPATH += . built-in-xlet ../baselib ../gui

# Input
HEADERS += *.h built-in-xlet/*.h

SOURCES += *.cpp built-in-xlet/*.cpp

unix:LIBS += -L$$ROOT_DIR/bin -lxivoclientbaselib -lxivoclientgui
win32:LIBS += -L$$ROOT_DIR/bin -lxivoclientbaselib1 -lxivoclientgui

win32 {
    LIBS += -lole32 -loleaut32 -luuid
}

QT += xml

RESOURCES += xivoclient.qrc
RC_FILE = xivoclient.rc

# this should just tell lrelease to compile these files
TRANSLATIONS = $$ROOT_DIR/i18n/xivoclient_fr.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_nl.ts
TRANSLATIONS += $$ROOT_DIR/i18n/xivoclient_de.ts

DESTDIR  = $$ROOT_DIR/bin

# Get the plugins dir from shell env
PLUGINDIR = $$system(echo -n $XIVOCLIENT_PLUGINDIR)
isEmpty( PLUGINDIR ) {
    PLUGINDIR = /usr/share/xivoclient/plugins
}
DEFINES += PLUGINDIR=\"\\\"$${PLUGINDIR}\\\"\"

# GNU/Linux = strip and pack.
# These commands are executed just after the final compilation of the executable.
# Doing this here instead of the main Makefile avoids stripping an already packed version ...
unix:CONFIG(release,debug|release) {
    QMAKE_POST_LINK += strip $(TARGET) ;
    QMAKE_POST_LINK += upx $(TARGET) ;
}

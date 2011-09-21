include(../common.pri)

TEMPLATE     = lib
win32:CONFIG += dll
unix:CONFIG  += plugin

INCLUDEPATH += .
INCLUDEPATH += $$ROOT_DIR/src/baselib
INCLUDEPATH += $$ROOT_DIR/src/xivoclient
INCLUDEPATH += $$ROOT_DIR/src/gui

DESTDIR      = $$ROOT_DIR/bin/plugins
macx:DESTDIR = $$ROOT_DIR/bin/xivoclient.app/Contents/Resources/plugins
LIBS        += -L$$ROOT_DIR/bin/

unix:LIBS   += -lxivoclientbaselib -lxivoclientgui
win32:LIBS  += -lxivoclientbaselib1 -lxivoclientgui1

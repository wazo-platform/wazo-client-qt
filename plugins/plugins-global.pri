include(../common.pri)

TEMPLATE     = lib
win32:CONFIG += dll
unix:CONFIG  += plugin

INCLUDEPATH += ../../baselib/src
DESTDIR      = ../../bin/plugins
macx:DESTDIR = ../../bin/xivoclient.app/Contents/Resources/plugins
LIBS        += -L../../bin/

unix:LIBS   += -lxivoclientbaselib
win32:LIBS  += -lxivoclientbaselib1

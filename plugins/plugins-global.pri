include(../common.pri)

TEMPLATE     = lib
win32:CONFIG = dll
unix:CONFIG  = plugin

# plugin shouldn't have to have require network normaly
# way to make plugin have to be reworked
#
QT          += network

INCLUDEPATH += ../../baselib/src
DESTDIR      = ../../bin/plugins
LIBS        += -L../../bin/

unix:LIBS   += -lxivoclientbaselib
win32:LIBS  += -lxivoclientbaselib1

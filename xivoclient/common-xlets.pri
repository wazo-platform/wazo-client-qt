include(common-xivoclient.pri)

TEMPLATE     = lib
win32:CONFIG += dll
unix:CONFIG  += plugin

INCLUDEPATH += $${BASELIB_DIR}/src
INCLUDEPATH += $${ROOT_DIR}/src/xletlib

DESTDIR      = $${BIN_DIR}/plugins
macx:DESTDIR = $${BIN_DIR}/xivoclient.app/Contents/Resources/plugins
LIBS        += -L$${BIN_DIR} -L$${BASELIB_DIR}/bin

unix:LIBS   += -lxivoclient -lxivoclientxlets
win32:LIBS  += -lxivoclient1 -lxivoclientxlets1

include(common-xivoclient.pri)

TEMPLATE     = lib
win32:CONFIG += dll
unix:CONFIG  += plugin

INCLUDEPATH += $${BASELIB_DIR}/src
INCLUDEPATH += $${ROOT_DIR}/src/xletlib

DESTDIR      = $${BIN_DIR}/plugins
macx:DESTDIR = $${BIN_DIR}/xivoclient.app/Contents/Resources/plugins

LIBS += -L$${BIN_DIR}
unix:LIBS += -lxivoclientxlets
win32 {
    release:LIBS += -lxivoclientxlets
    debug:LIBS += -lxivoclientxletsd
}

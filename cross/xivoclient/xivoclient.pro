include(../../common.pri)

include(../../qtaddons/qtsingleapplication/src/qtsingleapplication.pri)
include(../../qtaddons/qtcolorpicker/src/qtcolorpicker.pri)

TEMPLATE = app
QT += xml
CONFIG += uitools
# CONFIG += console # uncomment to get console on Win32

DESTDIR  = ../bin

BASELIB_DIR = ../../baselib
COMMON_DIR = ../../common
XIVOCLIENT_DIR = ../../xivoclient

RC_FILE = $${XIVOCLIENT_DIR}/appli.rc
RESOURCES = $${XIVOCLIENT_DIR}/appli.qrc
TRANSLATIONS = $${XIVOCLIENT_DIR}/xivoclient_fr.ts $${XIVOCLIENT_DIR}/qt_fr.ts


unix {
  LIBS += -L../bin/ -lxivoclientbaselib
}

win32 {
  LIBS += -L../bin/ -lxivoclientbaselib1
}

INCLUDEPATH += . $${COMMON_DIR}  $${XIVOCLIENT_DIR}/src  $${BASELIB_DIR}/src

HEADERS += $${XIVOCLIENT_DIR}/src/*.h
HEADERS += $${COMMON_DIR}/*.h

SOURCES += $${XIVOCLIENT_DIR}/src/*.cpp
SOURCES += $${COMMON_DIR}/*.cpp 

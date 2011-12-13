include(common-xivoclient-bin.pri)

# This changes the DLL name
TEMPLATE = lib

CONFIG   += dll hide_symbols

# Library version
VERSION = $${XIVOVER}

TARGET = pyxivoclient

INCLUDEPATH += $$system(ls -d /usr/include/python*)

SWIG_FILES = src/pyxivoclient_wrap.i
SWIG_OUTPUT = obj/pyxivoclient_wrap.cpp

pyxivoclient_wrap.depends = $${SWIG_FILES} src/pyxivoclient.h
pyxivoclient_wrap.target = $${SWIG_OUTPUT}
pyxivoclient_wrap.commands = swig -c++ -python -o $$pyxivoclient_wrap.target $${SWIG_FILES}

INCLUDEPATH += $${ROOT_DIR}/src
SOURCES += $${SWIG_OUTPUT}
QMAKE_EXTRA_TARGETS += pyxivoclient_wrap

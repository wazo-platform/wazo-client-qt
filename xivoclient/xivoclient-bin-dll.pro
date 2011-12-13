include(common-xivoclient-bin.pri)

TEMPLATE = lib
CONFIG   += dll hide_symbols

# Library version
VERSION = $${XIVOVER}

TARGET = $$qtLibraryTarget(_xivoclientwrapper)

INCLUDEPATH += $$system(ls -d /usr/include/python*)

SWIG_FILES = src/pythonwrapper.i
SWIG_OUTPUT = obj/pythonwrapper.cpp

pythonwrapper.depends = $${SWIG_FILES}
pythonwrapper.target = $${SWIG_OUTPUT}
pythonwrapper.commands = swig -c++ -python -o $$pythonwrapper.target $$pythonwrapper.depends

QMAKE_EXTRA_TARGETS += pythonwrapper

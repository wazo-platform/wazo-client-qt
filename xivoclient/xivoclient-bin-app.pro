include(common-xivoclient-bin.pri)

TEMPLATE = app
# CONFIG += console # uncomment to get console on Win32

TARGET = xivoclient

INCLUDEPATH += /usr/include/python2.6
message($$INCLUDEPATH)

SOURCES -= $${ROOT_DIR}/src/pythonwrapper.cpp
HEADERS -= $${ROOT_DIR}/src/pythonwrapper.h

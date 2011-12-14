include(common-xivoclient-bin.pri)

TEMPLATE = app
# CONFIG += console # uncomment to get console on Win32

TARGET = xivoclient

SOURCES -= $${ROOT_DIR}/src/pyxivoclient.cpp
HEADERS -= $${ROOT_DIR}/src/pyxivoclient.h

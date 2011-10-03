
CONFIG += qtestlib
TEMPLATE = app
TARGET = 
DEPENDPATH += . ..
INCLUDEPATH += . ../src
LIBS += ../../bin/libxivoclientbaselib.so

# Input
HEADERS += test_userinfo.h
SOURCES += main.cpp test_userinfo.cpp

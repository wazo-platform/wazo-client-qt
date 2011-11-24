include (common-baselib.pri)

CONFIG += qtestlib
TEMPLATE = app
QT -= gui
TARGET = baselib-tests
DESTDIR = $${BIN_DIR}

INCLUDEPATH += $${ROOT_DIR}/src/tests $${ROOT_DIR}/src

# Input
HEADERS += $${ROOT_DIR}/src/tests/*.h
SOURCES += $${ROOT_DIR}/src/tests/*.cpp

HEADERS += $${ROOT_DIR}/src/userinfo.h \
           $${ROOT_DIR}/src/xinfo.h
SOURCES += $${ROOT_DIR}/src/userinfo.cpp \
           $${ROOT_DIR}/src/xinfo.cpp

RCC_DIR = $$ROOT_DIR/obj
MOC_DIR = $$ROOT_DIR/obj
OBJECTS_DIR = $$ROOT_DIR/obj

# This adds lines 'include "<file>"' at the very beginning of the cpp file,
# thus overriding definitions of real classes by mock classes.
# This is only necessary for the inclusion by real classes.
# Mock classes inclusions can safely ignore this.
QMAKE_CXXFLAGS += -include tests/mock_baseengine.h \
                  -include tests/mock_phoneinfo.h \
                  -include tests/mock_channelinfo.h

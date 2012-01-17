include (common-baselib.pri)

CONFIG += qtestlib
TEMPLATE = app
QT -= gui

RCC_DIR = $$ROOT_DIR/obj
MOC_DIR = $$ROOT_DIR/obj
OBJECTS_DIR = $$ROOT_DIR/obj
DESTDIR = $${BIN_DIR}/tests

# For mocks
INCLUDEPATH += $${ROOT_DIR}/src/tests
# For tested classes
INCLUDEPATH += $${ROOT_DIR}/src

HEADERS += $${ROOT_DIR}/src/tests/*.h
SOURCES += $${ROOT_DIR}/src/tests/*.cpp

# This adds lines 'include "<file>"' at the very beginning of the cpp file,
# thus overriding definitions of real classes by mock classes.
# This is only necessary for the inclusion by real classes.
# Mock classes inclusions can safely ignore this.
QMAKE_CXXFLAGS += "-include tests/mock_baseengine.h" \
                  "-include tests/mock_phoneinfo.h" \
                  "-include tests/mock_channelinfo.h" \
                  "-include tests/mock_meetmeinfo.h"

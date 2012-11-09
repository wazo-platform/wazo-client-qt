include (common-baselib.pri)

CONFIG += qtestlib
TEMPLATE = app
QT -= gui

RCC_DIR = $$ROOT_DIR/obj
MOC_DIR = $$ROOT_DIR/obj
OBJECTS_DIR = $$ROOT_DIR/obj
DESTDIR = $${BIN_DIR}/tests

# Do not override the real definition of what we are testing
# This adds lines 'include "<file>"' at the very beginning of the cpp file,
# thus overriding definitions of real classes by mock classes.
# This is only necessary for the inclusion by real classes.
# Mock classes inclusions can safely ignore this.
QMAKE_CXXFLAGS += "-include mock_baseengine.h" \
                  "-include mock_phoneinfo.h" \
                  "-include mock_channelinfo.h" \
                  "-include mock_meetmeinfo.h"

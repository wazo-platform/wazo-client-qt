include (../../../common-tests.pri)

# Do not override the real definition of what we are testing
# This adds lines 'include "<file>"' at the very beginning of the cpp file,
# thus overriding definitions of real classes by mock classes.
# This is only necessary for the inclusion by real classes.
# Mock classes inclusions can safely ignore this.
QMAKE_CXXFLAGS += "-include mock_baseengine.h" \
                  "-include mock_phoneinfo.h" \
                  "-include mock_channelinfo.h" \
                  "-include mock_meetmeinfo.h"

TARGET = testsuite

SOURCES += $${ROOT_DIR}/src/storage/tests/main.cpp

INCLUDEPATH += $${ROOT_DIR}/src/storage/tests
INCLUDEPATH += $${ROOT_DIR}/src/storage/tests/mock
INCLUDEPATH += $${ROOT_DIR}/src/storage/tests/suite

# For mocks
HEADERS += $${ROOT_DIR}/src/storage/tests/mock/*.h
SOURCES += $${ROOT_DIR}/src/storage/tests/mock/*.cpp

# For tested classes
INCLUDEPATH += $${ROOT_DIR}/src
INCLUDEPATH += $${ROOT_DIR}/src/storage

HEADERS += $${ROOT_DIR}/src/storage/tests/suite/*.h
SOURCES += $${ROOT_DIR}/src/storage/tests/suite/*.cpp

HEADERS += $${ROOT_DIR}/src/storage/init_watcher.h
SOURCES += $${ROOT_DIR}/src/storage/init_watcher.cpp

# The real class we want to unit-test
HEADERS += $${ROOT_DIR}/src/storage/userinfo.h \
           $${ROOT_DIR}/src/storage/xinfo.h \
           $${ROOT_DIR}/src/xivoconsts.h
SOURCES += $${ROOT_DIR}/src/storage/userinfo.cpp \
           $${ROOT_DIR}/src/storage/xinfo.cpp \
           $${ROOT_DIR}/src/xivoconsts.cpp

QMAKE_CXXFLAGS = "-include userinfo.h" $${QMAKE_CXXFLAGS}

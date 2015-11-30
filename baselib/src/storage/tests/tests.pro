include (../../../common-tests.pri)

# Do not override the real definition of what we are testing
# This adds lines 'include "<file>"' at the very beginning of the cpp file,
# thus overriding definitions of real classes by mock classes.
# This is only necessary for the inclusion by real classes.
# Mock classes inclusions can safely ignore this.

TARGET = testsuite

SOURCES += $${ROOT_DIR}/src/storage/tests/main.cpp

INCLUDEPATH += $${ROOT_DIR}/src/storage/tests
INCLUDEPATH += $${ROOT_DIR}/src/storage/tests/suite

# For tested classes
INCLUDEPATH += $${ROOT_DIR}/src
INCLUDEPATH += $${ROOT_DIR}/src/storage

HEADERS += $${ROOT_DIR}/src/storage/tests/suite/*.h
SOURCES += $${ROOT_DIR}/src/storage/tests/suite/*.cpp

HEADERS += $${ROOT_DIR}/src/storage/init_watcher.h
SOURCES += $${ROOT_DIR}/src/storage/init_watcher.cpp

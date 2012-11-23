include (../../common-tests.pri)

TARGET = testsuite2

SOURCES += $${ROOT_DIR}/src/tests/test_src.cpp

INCLUDEPATH += $${ROOT_DIR}/src/tests
INCLUDEPATH += $${ROOT_DIR}/src/tests/suite

INCLUDEPATH += $${ROOT_DIR}/src

HEADERS += $${ROOT_DIR}/src/tests/suite/*.h
SOURCES += $${ROOT_DIR}/src/tests/suite/*.cpp

HEADERS += $${ROOT_DIR}/src/id_converter.h
SOURCES += $${ROOT_DIR}/src/id_converter.cpp

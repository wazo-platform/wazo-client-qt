include (../../../common-tests.pri)

TARGET = test_class_name

TEST_DIR = $${ROOT_DIR}/src/tests/test_class_name
TESTED_DIR = $${ROOT_DIR}/src

INCLUDEPATH += $${TESTED_DIR}
INCLUDEPATH += $${MOC_DIR}

SOURCES += $${TEST_DIR}/*.cpp
HEADERS += $${TESTED_DIR}/dependency.h
HEADERS += $${TESTED_DIR}/class_name.h
SOURCES += $${TESTED_DIR}/class_name.cpp

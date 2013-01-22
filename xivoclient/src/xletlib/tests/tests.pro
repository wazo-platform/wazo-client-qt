include (../../../common-tests.pri)

TARGET = test_xlet_lib

SOURCES += $${ROOT_DIR}/src/xletlib/tests/test_src.cpp

INCLUDEPATH += $${ROOT_DIR}/src/xletlib/tests
INCLUDEPATH += $${ROOT_DIR}/src/xletlib/tests/suite

INCLUDEPATH += $${ROOT_DIR}/src/xletlib

HEADERS += $${ROOT_DIR}/src/xletlib/tests/suite/*.h
SOURCES += $${ROOT_DIR}/src/xletlib/tests/suite/*.cpp

# HEADERS += $${ROOT_DIR}/src/xletlib/id_converter.h
# SOURCES += $${ROOT_DIR}/src/xletlib/id_converter.cpp

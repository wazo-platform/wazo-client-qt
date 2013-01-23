include (../../../common-tests.pri)

TARGET = test_xlet_lib

SOURCES += $${ROOT_DIR}/src/xletlib/tests/test_src.cpp

INCLUDEPATH += $${ROOT_DIR}/src/xletlib/tests
INCLUDEPATH += $${ROOT_DIR}/src/xletlib/tests/suite

INCLUDEPATH += $${ROOT_DIR}/src/xletlib
INCLUDEPATH += $${GIT_DIR}/baselib/src
INCLUDEPATH += $${GIT_DIR}/baselib/src/storage

HEADERS += $${ROOT_DIR}/src/xletlib/tests/suite/*.h
SOURCES += $${ROOT_DIR}/src/xletlib/tests/suite/*.cpp
SOURCES += $${GIT_DIR}/baselib/src/storage/phoneinfo.cpp
SOURCES += $${GIT_DIR}/baselib/src/storage/xinfo.cpp

SOURCES += $${ROOT_DIR}/src/xletlib/line_directory_entry.cpp
SOURCES += $${ROOT_DIR}/src/xletlib/directory_entry_manager.cpp

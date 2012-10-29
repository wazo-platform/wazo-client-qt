include (../../../common-tests.pri)

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

include (../../../../common-tests.pri)

TARGET = test-userinfo

# The real class we want to unit-test
HEADERS += $${ROOT_DIR}/src/storage/userinfo.h \
           $${ROOT_DIR}/src/storage/xinfo.h
SOURCES += $${ROOT_DIR}/src/storage/userinfo.cpp \
           $${ROOT_DIR}/src/storage/xinfo.cpp

# Do not override the real definition of what we are testing
QMAKE_CXXFLAGS = "-include ../userinfo.h" $${QMAKE_CXXFLAGS}

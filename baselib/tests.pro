include (common-baselib.pri)

CONFIG += qtestlib
TEMPLATE = app
QT -= gui
TARGET = baselib-tests
DESTDIR = $${BIN_DIR}

# Do not put anything before the tests directory
# It makes the mocks get priority over "official" headers
INCLUDEPATH += $${ROOT_DIR}/src/tests $${ROOT_DIR}/src

# Input
HEADERS += $${ROOT_DIR}/src/tests/*.h
SOURCES += $${ROOT_DIR}/src/tests/*.cpp

HEADERS += $${ROOT_DIR}/src/userinfo.h \
           $${ROOT_DIR}/src/xinfo.h
SOURCES += $${ROOT_DIR}/src/userinfo.cpp \
           $${ROOT_DIR}/src/xinfo.cpp

RCC_DIR = $$ROOT_DIR/obj
MOC_DIR = $$ROOT_DIR/obj
OBJECTS_DIR = $$ROOT_DIR/obj

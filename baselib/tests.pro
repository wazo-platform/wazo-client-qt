include (common-baselib.pri)

CONFIG += qtestlib
TEMPLATE = app
QT -= gui
TARGET = baselib-tests
DESTDIR = $${BIN_DIR}
# DEPENDPATH += . ..
INCLUDEPATH += $${ROOT_DIR}/src $${ROOT_DIR}/src/tests
LIBS += -L$${BIN_DIR} -lxivoclient

# Input
HEADERS += $${ROOT_DIR}/src/tests/*.h
SOURCES += $${ROOT_DIR}/src/tests/*.cpp

MOC_DIR = $$ROOT_DIR/obj
OBJECTS_DIR = $$ROOT_DIR/obj
RCC_DIR = $$ROOT_DIR/obj

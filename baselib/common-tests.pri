include (common-baselib.pri)

CONFIG += qtestlib
TEMPLATE = app
QT -= gui

RCC_DIR = $$ROOT_DIR/obj
MOC_DIR = $$ROOT_DIR/obj
OBJECTS_DIR = $$ROOT_DIR/obj
DESTDIR = $${BIN_DIR}/tests

QT+=testlib
TEMPLATE = app
#QT -= gui

RCC_DIR = $$ROOT_DIR/obj
MOC_DIR = $$ROOT_DIR/obj
OBJECTS_DIR = $$ROOT_DIR/obj
DESTDIR = $${BIN_DIR}/tests

gmock {
    GMOCK_SOURCES = /usr/src/gmock/src/gmock-all.cc \
                    /usr/src/gtest/src/gtest-all.cc

    QMAKE_EXTRA_COMPILERS += gmock_compiler
    gmock_compiler.input = GMOCK_SOURCES
    gmock_compiler.output = $${GIT_DIR}/${QMAKE_FILE_BASE}.o
    gmock_compiler.commands = g++ \
                              -I/usr/src/gtest \
                              -I/usr/src/gmock \
                              -c ${QMAKE_FILE_IN} \
                              -o ${QMAKE_FILE_OUT}
    gmock_compiler.CONFIG = no_link

    QMAKE_EXTRA_COMPILERS += gmock_linker
    gmock_linker.depends = $${GIT_DIR}/gmock-all.o \
                           $${GIT_DIR}/gtest-all.o
    gmock_linker.input = GMOCK_SOURCES
    gmock_linker.output = $${GIT_DIR}/libgmock.a
    gmock_linker.commands = ar -rv ${QMAKE_FILE_OUT} $${GIT_DIR}/gmock-all.o $${GIT_DIR}/gtest-all.o
    gmock_linker.CONFIG = combine explicit_dependencies no_link target_predeps

    LIBS += $${GIT_DIR}/libgmock.a
} else {
    LIBS += -lgmock -lgtest
}

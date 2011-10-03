exists(../common.pri) {
    include(../common.pri)
    # GIT_DIR defined in common.pri
    ROOT_DIR = $${GIT_DIR}/baselib
} else {
    ROOT_DIR = .
    XIVOVER = 1.2

    DEFINES += XIVOVER=\"\\\"$${XIVOVER}\\\"\"
    DEFINES += GIT_HASH=\"\\\"unknown\\\"\"
    DEFINES += GIT_DATE=\"\\\"unknown\\\"\"
    DEFINES += DATEBUILD=\"\\\"unknown\\\"\"
}
BIN_DIR = $${ROOT_DIR}/bin

TEMPLATE    = lib
CONFIG      += dll

TARGET      = $$qtLibraryTarget(xivoclient)

JSON_JSONQT_DIR = $${ROOT_DIR}/json_jsonqt/lib

# Library version
VERSION      = $${XIVOVER}
DEFINES     += BASELIB_LIBRARY
INCLUDEPATH += $${JSON_JSONQT_DIR}

HEADERS     += $${ROOT_DIR}/src/*.h
HEADERS     += $${ROOT_DIR}/src/dstore/*.h
HEADERS     += $${JSON_JSONQT_DIR}/*.h

SOURCES     += $${ROOT_DIR}/src/*.cpp
SOURCES     += $${ROOT_DIR}/src/dstore/*.cpp
SOURCES     += $${JSON_JSONQT_DIR}/*.cpp

DESTDIR     = $${BIN_DIR}

QT += network

TRANSLATIONS  = $${ROOT_DIR}/baselib_fr.ts
TRANSLATIONS += $${ROOT_DIR}/baselib_nl.ts

QMAKE_EXTRA_COMPILERS += updateqm
updateqm.input = TRANSLATIONS
updateqm.output = $$ROOT_DIR/obj/${QMAKE_FILE_BASE}.qm
updateqm.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
# no_link = not included for linking
# target_predep = will be compiled before the executable
updateqm.CONFIG += no_link target_predeps

RESOURCES += $${ROOT_DIR}/baselib.qrc

MOC_DIR = $$ROOT_DIR/obj
OBJECTS_DIR = $$ROOT_DIR/obj
RCC_DIR = $$ROOT_DIR/obj

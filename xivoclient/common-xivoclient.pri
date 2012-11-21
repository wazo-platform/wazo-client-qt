include (../common.pri)

ROOT_DIR = $${GIT_DIR}/xivoclient
BASELIB_DIR = $${GIT_DIR}/baselib
BIN_DIR = $${GIT_DIR}/bin

# regenerate qm when needed

QMAKE_EXTRA_COMPILERS += updateqm
updateqm.input = TRANSLATIONS
updateqm.output = $$ROOT_DIR/obj/${QMAKE_FILE_BASE}.qm
updateqm.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
# no_link = not included for linking
# target_predep = will be compiled before the executable
updateqm.CONFIG += no_link target_predeps

MOC_DIR = $$ROOT_DIR/obj
OBJECTS_DIR = $$ROOT_DIR/obj
RCC_DIR = $$ROOT_DIR/obj
UI_DIR = $$ROOT_DIR/obj

LIBS += -L$${BIN_DIR}
unix:LIBS += -lxivoclient
win32 {
    debug:LIBS += -lxivoclientd
    release:LIBS += -lxivoclient
    LIBS += -lole32 -loleaut32 -luuid
}

QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O1

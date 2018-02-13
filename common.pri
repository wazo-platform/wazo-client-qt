! include(versions.mak) {
    error(Missing file versions.mak. Please do 'qmake'.)
}

DEBUG_MODE = $$(DEBUG)
CONFIG -= debug
contains(DEBUG_MODE, yes) {
    CONFIG += debug
    CONFIG -= release
    message(">> Debug Build <<")
}

FUNCTESTS_MODE = $$(FUNCTESTS)
contains(FUNCTESTS_MODE, yes) {
    DEFINES += FUNCTESTS
}

# add everything correctly escaped as a string

DEFINES += XC_VERSION=\"\\\"$${XC_VERSION}\\\"\"
DEFINES += GIT_HASH=\"\\\"$${GIT_HASH}\\\"\"
DEFINES += GIT_DATE=\"\\\"$${GIT_DATE}\\\"\"
DEFINES += DATEBUILD=\"\\\"$${DATEBUILD}\\\"\"

# Raise error on link if symbols are undefined
linux-g++ {
    QMAKE_LFLAGS += -Wl,-z,defs
}

# Some useful warnings

QMAKE_CXXFLAGS += -Wall \
                  -Wextra \
                  -Wundef \
                  -Wcast-qual \
                  -Wcast-align \
                  -Wwrite-strings \
                  -Winit-self \
                  -Wstrict-aliasing

# Fix missing LRELEASE path
isEmpty(QMAKE_LRELEASE) {
    win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

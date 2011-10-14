# Test if we are in the xivoclient context
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

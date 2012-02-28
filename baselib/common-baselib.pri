# Test if we are in the xivoclient context
exists(../common.pri) {
    include(../common.pri)
    # GIT_DIR defined in common.pri
    clear(ROOT_DIR)
    ROOT_DIR = $${GIT_DIR}/baselib
    CONFIG += xivoclient
    BIN_DIR = $${GIT_DIR}/bin
} else {
    ROOT_DIR = $${PWD}

    DEFINES += XC_VERSION=\"\\\"unknown\\\"\"
    DEFINES += GIT_HASH=\"\\\"unknown\\\"\"
    DEFINES += GIT_DATE=\"\\\"unknown\\\"\"
    DEFINES += DATEBUILD=\"\\\"unknown\\\"\"

    BIN_DIR = $${ROOT_DIR}/bin
}

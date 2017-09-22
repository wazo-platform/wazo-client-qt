# Test if we are in the wazoclient context
exists(../common.pri) {
    include(../common.pri)
    # GIT_DIR defined in common.pri
    ROOT_DIR = $${GIT_DIR}/baselib
    CONFIG += wazoclient
    BIN_DIR = $${GIT_DIR}/bin
} else {
    clear(ROOT_DIR)
    ROOT_DIR = $${PWD}

    DEFINES += XC_VERSION=\"\\\"unknown\\\"\"
    DEFINES += GIT_HASH=\"\\\"unknown\\\"\"
    DEFINES += GIT_DATE=\"\\\"unknown\\\"\"
    DEFINES += DATEBUILD=\"\\\"unknown\\\"\"

    BIN_DIR = $${ROOT_DIR}/bin
}

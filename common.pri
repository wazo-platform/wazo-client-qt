XIVOVER = 1.1
GIT_HASH = to_fill
GIT_DATE = to_fill

include(versions.pri)

DATEBUILD = $$system(LANG= date +%Y-%m-%dT%H:%M:%S)
DEBUGON = $$system(echo -n $DEBUG)

contains(DEBUGON, "yes" ) {
        CONFIG += debug
        message(">> Debug Build <<")
}

# take care of inferior(s) system(s) {

isEmpty( DATEBUILD ) {
        DATEBUILD = "ice age"
}

# }

!build_pass:message('XiVO version    :' $${XIVOVER})
!build_pass:message('git commit hash :' $${GIT_HASH})
!build_pass:message('git commit date :' $${GIT_DATE})
!build_pass:message('build date      :' $${DATEBUILD})

# add everything correctly escaped as a string

QMAKE_CXXFLAGS += -DXIVOVER=\"\\\"$${XIVOVER}\\\"\"
QMAKE_CXXFLAGS += -DGIT_HASH=\"\\\"$${GIT_HASH}\\\"\"
QMAKE_CXXFLAGS += -DGIT_DATE=\"\\\"$${GIT_DATE}\\\"\"
QMAKE_CXXFLAGS += -DDATEBUILD=\"\\\"$${DATEBUILD}\\\"\"

# regenerate qm when needed

QMAKE_EXTRA_COMPILERS += updateqm
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
PRE_TARGETDEPS += compiler_updateqm_make_all

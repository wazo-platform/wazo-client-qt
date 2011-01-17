XIVOVER = 1.1
GITVER = to_fill
GITDATE = to_fill

DATEBUILD = $$system(LANG= date +%Y-%m-%dT%H:%M:%S)
SERVER_VERSION_REQUIRED = 5650
DEBUGON= $$system(echo -n $DEBUG)

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
!build_pass:message('git commit hash :' $${GITVER})
!build_pass:message('git commit date :' $${GITDATE})
!build_pass:message('build date      :' $${DATEBUILD})

# add everything correctly escaped as a string

QMAKE_CXXFLAGS += -DXIVOVER=\"\\\"$${XIVOVER}\\\"\"
QMAKE_CXXFLAGS += -DGITVER=\"\\\"$${GITVER}\\\"\"
QMAKE_CXXFLAGS += -DGITDATE=\"\\\"$${GITDATE}\\\"\"
QMAKE_CXXFLAGS += -DDATEBUILD=\"\\\"$${DATEBUILD}\\\"\"

# regenerate qm when needed

QMAKE_EXTRA_COMPILERS += updateqm
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
PRE_TARGETDEPS += compiler_updateqm_make_all

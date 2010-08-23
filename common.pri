XIVOVER = 1.2

SVNVER = $$system(LANG= svn info | sed -n \"s/Revision: //p\")
DATEBUILD = $$system(LANG= date +%Y-%m-%dT%H:%M:%S)

#CONFIG += debug

# take care of inferior(s) system(s) {

isEmpty( SVNVER ) {
  SVNVER = 9999
}
  
isEmpty( DATEBUILD ) {
  DATEBUILD = "ice age"
}

# }

!build_pass:message('XiVO version:' $${XIVOVER})
!build_pass:message('svn version:' $${SVNVER})
!build_pass:message('build date:'  $${DATEBUILD})

# add everything correctly escaped as a string

QMAKE_CXXFLAGS += -DXIVOVER=\"\\\"$${XIVOVER}\\\"\"
QMAKE_CXXFLAGS += -DSVNVER=\"\\\"$${SVNVER}\\\"\"
QMAKE_CXXFLAGS += -DDATEBUILD=\"\\\"$${DATEBUILD}\\\"\"

# regenerate qm when needed

QMAKE_EXTRA_COMPILERS += updateqm
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = lrelease ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link
PRE_TARGETDEPS += compiler_updateqm_make_all


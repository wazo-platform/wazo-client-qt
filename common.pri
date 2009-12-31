XIVOVER = 1.1

SVNVER = $$system(LANG= svn info | sed -n \"s/Revision: //p\")
DATEBUILD =  $$system(LANG= date)

# take care of inferior(s) system(s) {

  isEmpty( SVNVER ) {
    SVNVER = 9999
  }
  
  isEmpty( DATEBUILD ) {
    DATEBUILD = "ice age"
  }

# }

!build_pass:message('XIVO version:' $${XIVOVER})
!build_pass:message('svn version:' $${SVNVER})
!build_pass:message('build date:'  $${DATEBUILD})

# add everything correctly escaped as a string

QMAKE_CXXFLAGS += -DXIVOVER=\"\\\"$${XIVOVER}\\\"\"
QMAKE_CXXFLAGS += -DSVNVER=\"\\\"$${SVNVER}\\\"\"
QMAKE_CXXFLAGS += -DDATEBUILD=\"\\\"$${DATEBUILD}\\\"\"

exists(versions.mak) {
    include(versions.mak)
} else {
    error(Missing file versions.mak. Please do 'make versions')
}

DEBUGON = $$system(echo -n $DEBUG)

CONFIG -= debug
contains(DEBUGON, "yes" ) {
    CONFIG += debug
    CONFIG -= release
    message(">> Debug Build <<")
}

!build_pass:message('XiVO version    :' $${XIVOVER})
!build_pass:message('git commit hash :' $${GIT_HASH})
!build_pass:message('git commit date :' $${GIT_DATE})
!build_pass:message('build date      :' $${DATEBUILD})

# add everything correctly escaped as a string

DEFINES += XIVOVER=\"\\\"$${XIVOVER}\\\"\"
DEFINES += GIT_HASH=\"\\\"$${GIT_HASH}\\\"\"
DEFINES += GIT_DATE=\"\\\"$${GIT_DATE}\\\"\"
DEFINES += DATEBUILD=\"\\\"$${DATEBUILD}\\\"\"

_XIVOVER_ = 1.1
_SVNVER_ = 1
include(xivoclient/versions.pri)
XIVOVER = '\\"$${_XIVOVER_}\\"'
SVNVER  = '\\"$${_SVNVER_}\\"'
DATEBUILD = '\\"$${_DATEBUILD_}\\"'
message('XIVO version:' $${XIVOVER})
message(' svn version:' $${SVNVER})
message('  build date:' $${DATEBUILD})
DEFINES += XIVOVER=\"$${XIVOVER}\"
DEFINES += SVNVER=\"$${SVNVER}\"
DEFINES += DATEBUILD=\"$${DATEBUILD}\"

CONFIG += release
CONFIG -= debug
#CONFIG += debug
CONFIG += uitools
CONFIG += x86 ppc


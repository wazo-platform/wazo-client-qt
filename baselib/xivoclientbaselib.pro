# $Id$
COMMONDIR = ../common
JSON_JSONQT_DIR = ../3rdparty/json_jsonqt/lib

_XIVOVER_ = 1.1
_SVNVER_ = 1
include(../xivoclient/versions.pro)
XIVOVER = '\\"$${_XIVOVER_}\\"'
SVNVER  = '\\"$${_SVNVER_}\\"'
DATEBUILD = '\\"$${_DATEBUILD_}\\"'
message('XIVO version:' $${XIVOVER})
message(' svn version:' $${SVNVER})
message('  build date:' $${DATEBUILD})
DEFINES += XIVOVER=\"$${XIVOVER}\"
DEFINES += SVNVER=\"$${SVNVER}\"
DEFINES += DATEBUILD=\"$${DATEBUILD}\"

VERSION     = 1.1

TEMPLATE    = lib
CONFIG      += release
INCLUDEPATH += $${COMMONDIR} $${JSON_JSONQT_DIR}
HEADERS     = xlet.h   baseengine.h
HEADERS     += $${JSON_JSONQT_DIR}/*.h
SOURCES     = xlet.cpp baseengine.cpp
SOURCES     += $${JSON_JSONQT_DIR}/*.cpp

DESTDIR     = ../bin

QT += network


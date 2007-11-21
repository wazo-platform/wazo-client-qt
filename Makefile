# $Revision$
# $Date$

# path-less commands are better handled when compiling on win32
QMAKE?=qmake
LRELEASE?=lrelease

# Tool to build executables 
MAKENSIS=C:\Program Files\NSIS\makensis.exe
XIVOVER:=$(shell cat ../VERSION)
SVNVER:=$(shell svn info | sed -n "/Last.Changed.Rev/s/.* //p")

default:

xc:
	${QMAKE} xivoclient/xivoclient.pro   -o xivoclient/Makefile
	${LRELEASE} xivoclient/xivoclient_fr.ts
	cd xivoclient && make

sb:
	${QMAKE} switchboard/switchboard.pro -o switchboard/Makefile
	${LRELEASE} switchboard/switchboard_fr.ts
	cd switchboard && make

all: xc sb

upx:
	upx */release/*.exe

nsis:
	${MAKENSIS} delivery/switchboard.nsi
	${MAKENSIS} delivery/xivoclient.nsi
	mv delivery/switchboard-setup.exe switchboard-setup-${XIVOVER}-${SVNVER}-win32.exe
	mv delivery/xivoclient-setup.exe xivoclient-setup-${XIVOVER}-${SVNVER}-win32.exe

allwin: all upx nsis


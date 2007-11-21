# $Revision$
# $Date$

# path-less commands are better handled when compiling on win32
QMAKE?=qmake
LRELEASE?=lrelease
UPXRUN?=upx

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
	${UPXRUN} */release/*.exe

nsis:
	${MAKENSIS} delivery/switchboard.nsi
	${MAKENSIS} delivery/xivoclient.nsi
	cp delivery/switchboard-setup.exe switchboard-setup-${XIVOVER}-${SVNVER}-win32.exe
	cp delivery/xivoclient-setup.exe xivoclient-setup-${XIVOVER}-${SVNVER}-win32.exe

all-win32: all upx nsis

# export UPXRUN=/Users/proformatique/upx-3.01-src/src/upx.out
all-macos: all
	${UPXRUN} */*.app/Contents/MacOS/*
	cp xivoclient/xivoclient.app/Contents/MacOS/xivoclient xivoclient-${XIVOVER}-${SVNVER}-macos
	cp switchboard/switchboard.app/Contents/MacOS/switchboard switchboard-${XIVOVER}-${SVNVER}-macos


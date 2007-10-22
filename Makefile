# $Revision$
# $Date$

# path-less commands are better handled when compiling on win32
QMAKE?=qmake
LRELEASE?=lrelease

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

allwin: all upx


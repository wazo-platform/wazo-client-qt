# $Revision$
# $Date$

QMAKE?=/usr/bin/qmake
LRELEASE?=/usr/bin/lrelease
UPX?=/usr/bin/upx

default:

xc:
	${QMAKE} xivoclient/xivoclient.pro   -o xivoclient/Makefile
	${LRELEASE} xivoclient/xivoclient_fr.ts
	make -C xivoclient

sb:
	${QMAKE} switchboard/switchboard.pro -o switchboard/Makefile
	${LRELEASE} switchboard/switchboard_fr.ts
	make -C switchboard

all: xc sb

upx:
	${UPX} */release/*.exe


# $Revision$
# $Date$

QMAKE?=/usr/bin/qmake
LRELEASE?=/usr/bin/lrelease
UPX?=/usr/bin/upx

default:

xivoclient:
	${QMAKE} xivoclient/xivoclient.pro   -o xivoclient/Makefile
	${LRELEASE} xivoclient/xivoclient_fr.ts
	make -C xivoclient

switchboard:
	${QMAKE} switchboard/switchboard.pro -o switchboard/Makefile
	${LRELEASE} switchboard/switchboard_fr.ts
	make -C switchboard

all: xivoclient switchboard

upx:
	${UPX} */release/*.exe


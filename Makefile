# $Revision$
# $Date$

QMAKE?=qmake
LRELEASE?=lrelease
UPXRUN?=upx

# Version definitions
XIVOVER:=$(shell cat ../VERSION)
SVNVER:=$(shell svn info | sed -n "/Last.Changed.Rev/s/.* //p")

default: help

help:
	@echo
	@echo "Please choose your target according to the OS you are using :"
	@echo " -- to build Linux binaries or debian packages :"
	@echo "          make all-linux "
	@echo "          make all-debian "
	@echo " -- to build Windows binaries (application + NSIS setup) :"
	@echo "          make all-win32 "
	@echo " -- to build Mac OS X binaries (Universal Binary applications + .dmg packages) :"
	@echo "          make all-macos "
	@echo "Thanks."
	@echo

# LINUX targets
all-linux: linux-xivoclient linux-switchboard

linux-%:
	cd $* && ${QMAKE} && ${LRELEASE} $*_fr.ts && make
	strip $*/$* || true
	${UPXRUN} $*/$* || true


# WIN32 targets
MAKENSIS=/cygdrive/c/Program\ Files/NSIS/makensis.exe

all-win32: win32-xivoclient win32-switchboard

win32-%:
	cd $* && ${QMAKE} $*.pro && ${LRELEASE} $*_fr.ts && make -f Makefile.Release
	${UPXRUN} $*/release/$*.exe
	${MAKENSIS} delivery/$*.nsi
	cp delivery/$*-setup.exe $*-setup-${XIVOVER}-${SVNVER}-win32.exe



# MACOS targets
# export UPXRUN=/Users/proformatique/upx-3.01-src/src/upx.out

all-macos: macos-xivoclient macos-switchboard

macos-%:
	${QMAKE} $*/$*.pro -o $*/Makefile
	${LRELEASE} $*/$*_fr.ts
	cd $* && make
	${UPXRUN} $*/$*.app/Contents/MacOS/$*
	hdiutil create $*-${XIVOVER}-${SVNVER}.dmg -srcfolder $*/$*.app -format UDBZ



# DEBIAN targets

all-debian : debian-xivoclient debian-switchboard

debian-%:
	@echo "## Building Debian Package for : $*"
	@echo "## Changelog will be updated with ${NAME} <${DEBEMAIL}>"
	cd $* && dch -v ${XIVOVER}~svn${SVNVER}-1 "New upstream release."
	rm -rf pf-xivo-cti-$**
	mkdir pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER}
	cp -rp $*/debian pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER}/debian
	cd $* && tar zcf ../pf-xivo-cti-$*_${XIVOVER}~svn${SVNVER}.orig.tar.gz $*
	cp $*/$* pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER}
	cd pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER} && \
		DEB_AUTO_UPDATE_DEBIAN_CONTROL=yes fakeroot debian/rules clean && \
		dpkg-buildpackage -us -uc -tc -rfakeroot

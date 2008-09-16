# $Revision$
# $Date$

QMAKE?=qmake
LRELEASE?=lrelease
UPXRUN?=upx

# WIN32 targets only
MAKENSIS=/cygdrive/c/Program\ Files/NSIS/makensis.exe
UPXWIN=/cygdrive/c/upx303w/upx.exe

# Versions Definitions
-include versions.mak

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
# kind of dirtier than "all-linux: versions-xivoclient linux-xivoclient"
# but allows the 'include versions.mak' to be reloaded once it has been set
all-linux:
	@make versions-xivoclient
	@make linux-xivoclient

linux-%:
	@echo versions ${_XIVOVER_}-${_SVNVER_}
	@make -C $* distclean || true
	@cd $* && ${QMAKE} && ${LRELEASE} $*_fr.ts qt_fr.ts && make
	@strip $*/$* || true
	@${UPXRUN} $*/$* || true

all-win32:
	@echo "          make versions-xivoclient  (under Cygwin)"
	@echo "          make win32-xivoclient     (under Qt prompt)"
	@echo "          make win32pack-xivoclient (under Cygwin)"

# to be executed under a bash/cygwin-like terminal
versions-%:
	@svn up
	@touch common/xivoconsts.h $*/mainwidget.cpp
	@echo versions ${_XIVOVER_}-${_SVNVER_}
	@rm -f $*/versions.pro
	@echo -n "_SVNVER_ = '" >> $*/versions.pro
	@LANG=C svn info | grep "Last Changed Rev" | sed "s/.*: //" | tr -d '\n' >> $*/versions.pro
	@echo "'" >> $*/versions.pro
	@grep -h "VER_ =" $*/*.pro | sort -r | head -2 > versions.mak

# to be executed under a mingw/dos-like terminal
win32-%:
	@cd $* && ${QMAKE} $*.pro && ${LRELEASE} $*_fr.ts && make -f Makefile.Release

# to be executed under a bash/cygwin-like terminal
win32pack-%:
	@${UPXWIN} $*/release/$*.exe
	@${MAKENSIS} delivery/$*.nsi
	@mv delivery/$*-setup-win32.exe $*-setup-${_XIVOVER_}-${_SVNVER_}-win32.exe


# MACOS targets
# export UPXRUN=/Users/proformatique/upx-3.01-src/src/upx.out

all-macos: macos-xivoclient

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

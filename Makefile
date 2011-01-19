# $Revision: 8994 $
# $Date: 2010-08-24 12:53:00 +0200 (Tue, 24 Aug 2010) $

QMAKE?=qmake
UPXRUN?=upx
ECHO?=/bin/echo
JOPT?=-j1
DOXYGEN?=doxygen

# WIN32 targets only
MAKENSIS=/cygdrive/c/Program\ Files/NSIS/makensis.exe
UPXWIN?=/cygdrive/c/upx303w/upx.exe

# Versions Definitions
-include versions.mak
-include uname.mak

default: help

help: guessos
	@echo
	@echo "Please choose your target according to the OS you are using :"
	@echo " -- to build Linux binaries or debian packages :"
	@echo "          make all-linux "
	@echo "          make all-debian "
	@echo " -- to build Windows binaries (application + NSIS setup) :"
	@echo "          make all-win32 "
	@echo " -- to build Mac OS X binaries (Universal Binary applications + .dmg packages) :"
	@echo "          make all-macos "
	@echo ""
	@echo "You can also build docs using doxygen :"
	@echo "          make doc"
	@echo "Thanks."

guessos:
	@${ECHO} -n "_UNAME_ = " > uname.mak
	@uname -s | sed "s/Linux/linux/;s/CYGWIN.*/win32/;s/Darwin/macos/" >> uname.mak

all:
	@make -s guessos
	@make allbyos

allbyos:
	@echo "Will compile for target ${_UNAME_} : all-${_UNAME_}"
	@make all-${_UNAME_}

# LINUX targets
# kind of dirtier than "all-linux: versions linux-xivoclient"
# but allows the 'include versions.mak' to be reloaded once it has been set
all-linux:
	@make versions
	@make linux-baselib
	@make linux-xivoclient
	@make linux-plugins
	@if [ -z "${DEBUG}" ] ; then make stripandpack-xivoclient ; fi

distclean:
	@make clean-xivoclient
	@make clean-baselib
	@make clean-plugins
	find . -name "moc_*" -print0 | xargs -0 rm -f
	find . -name "*.o" -print0 | xargs -0 rm -f
	rm -rf bin ; mkdir -p bin/plugins

clean-%:
	@make -C $* distclean || true

linux-%:
	@cd $* && ${QMAKE} && make ${JOPT}

stripandpack-%:
	@strip bin/$* || true
	@${UPXRUN} bin/$* || true

all-win32:
	@make -s versions
	@echo "   (under Qt prompt) :"
	@pwd | sed "s#/#\\\\#g;s#^#      cd C:\\\\cygwin#"
	@echo "      make win32-baselib win32-xivoclient win32-plugins"
	@echo "   (back here under Cygwin) :"
	@echo "      make win32pack-xivoclient"

# to be executed under a bash/cygwin-like terminal
versions:
	@${ECHO} -n "version (before update) : " && make -s displayversions
	@rm -f versions.pri
	@${ECHO} -n "GIT_HASH = " >> versions.pri
	@git log -1 --pretty=%h $(git rev-list HEAD --max-count=1) >> versions.pri
	@${ECHO} -n "GIT_DATE = " >> versions.pri
	@git log -1 --pretty=%ct $(git rev-list HEAD --max-count=1) >> versions.pri
	@cp versions.pri versions.mak
	@grep ^XIVOVER common.pri >> versions.mak
	@${ECHO} -n "version (after update) : " && make -s displayversions

displayversions:
	@${ECHO} ${XIVOVER}-${GIT_DATE}-${GIT_HASH}

# to be executed under a mingw/dos-like terminal
win32-%:
	@cd $* && ${QMAKE} && make release

# to be executed under a bash/cygwin-like terminal
win32pack-%:
	@${UPXWIN} $*/release/$*.exe || true
	@rm -f delivery/settings.nsh
	@${ECHO} "!define XIVOVER ${_XIVOVER_}" >> delivery/settings.nsh
	@${ECHO} "!define SVNVER ${_SVNVER_}" >> delivery/settings.nsh
	@${MAKENSIS} delivery/$*.nsi
	@mv delivery/$*-setup-win32.exe $*-setup-${_XIVOVER_}-${_SVNVER_}-win32.exe

win32packdyn-%:
	@${UPXWIN} $*/release/$*.exe || true
	@rm -f delivery/settings.nsh
	@${ECHO} "!define XIVOVER ${_XIVOVER_}" >> delivery/settings.nsh
	@${ECHO} "!define SVNVER ${_SVNVER_}" >> delivery/settings.nsh
	@${ECHO} "!define QTBINPATH c:\Qt\4.5.2\bin" >> delivery/settings.nsh
	@${MAKENSIS} delivery/$*.nsi
	@mv delivery/$*-setup-win32.exe $*-setup-${_XIVOVER_}-${_SVNVER_}-win32.exe

# MACOS targets
# (man hdiutil) "-format UDBZ" works only >= 10.4, thus "-format UDZO"
# export UPXRUN=/Users/proformatique/upx-3.01-src/src/upx.out

all-macos:
	@make versions
	@make macos-xivoclient

macos-%:
	@cd $* && ${QMAKE} -spec macx-g++ $*.pro -o Makefile && make ${JOPT}
	@strip $*/$*.app/Contents/MacOS/$*
	@${UPXRUN} $*/$*.app/Contents/MacOS/$*
	@mkdir -p $*/$*.app/Contents/Resources/French.lproj
	@hdiutil create $*-${_XIVOVER_}-${_SVNVER_}.dmg -srcfolder $*/$*.app -format UDZO


# DEBIAN targets

all-debian : debian-xivoclient

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

# build doc

doc:	common xivoclient
	${DOXYGEN} xivoclient/Doxyfile 

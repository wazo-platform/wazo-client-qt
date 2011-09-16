QMAKE?=qmake
ECHO?=/bin/echo
DOXYGEN?=doxygen

# WIN32 targets only
MAKENSIS=/cygdrive/c/Program\ Files/NSIS/makensis.exe
UPXWIN?=/cygdrive/c/upx303w/upx.exe

# Versions Definitions
-include versions.mak
-include uname.mak

default: all

help: guessos
	@echo
	@echo "Please choose your target according to the OS you are using :"
	@echo " -- to build Linux binaries or debian packages :"
	@echo "          $(MAKE) all-linux "
	@echo "          $(MAKE) all-debian "
	@echo " -- to build Windows binaries (application + NSIS setup) :"
	@echo "          $(MAKE) all-win32 "
	@echo " -- to build Mac OS X binaries (Universal Binary applications + .dmg packages) :"
	@echo "          $(MAKE) all-macos "
	@echo ""
	@echo "You can also build docs using doxygen :"
	@echo "          $(MAKE) doc"
	@echo "Thanks."

guessos:
	@${ECHO} -n "_UNAME_ = " > uname.mak
	@uname -s | sed "s/Linux/linux/;s/CYGWIN.*/win32/;s/Darwin/macos/" >> uname.mak

all:
	@$(MAKE) -s guessos
	@$(MAKE) allbyos

allbyos:
	@echo "Will compile for target ${_UNAME_} : all-${_UNAME_}"
	@$(MAKE) all-${_UNAME_}

# LINUX targets
# kind of dirtier than "all-linux: versions linux-xivoclient"
# but allows the 'include versions.mak' to be reloaded once it has been set
all-linux:
	@$(MAKE) linux-baselib
	@$(MAKE) linux-plugins
	@$(MAKE) linux-xivoclient

distclean:
	@$(MAKE) clean-xivoclient
	@$(MAKE) clean-baselib
	@$(MAKE) clean-plugins
	find . -name "moc_*" -print0 | xargs -0 rm -f
	find . -name "*.o" -print0 | xargs -0 rm -f
	rm -rf bin ; mkdir -p bin/plugins

clean-%:
	@$(MAKE) -C $* distclean || true

linux-%:
	@cd $* && ${QMAKE} && $(MAKE)

all-win32:
	@$(MAKE) -s versions
	@echo "   (under Qt prompt) :"
	@pwd | sed "s#/#\\\\#g;s#^#      cd C:\\\\cygwin#"
	@echo "      $(MAKE) win32-baselib win32-xivoclient win32-plugins"
	@echo "   (back here under Cygwin) :"
	@echo "      $(MAKE) win32pack-xivoclient"

# to be executed under a bash/cygwin-like terminal
versions:
	@${ECHO} -n "version (before update) : " && $(MAKE) -s displayversions
	@rm -f versions.pri
	@${ECHO} -n "GIT_HASH = " >> versions.pri
	@git log -1 --pretty=%h $(git rev-list HEAD --max-count=1) >> versions.pri
	@${ECHO} -n "GIT_DATE = " >> versions.pri
	@git log -1 --pretty=%ct $(git rev-list HEAD --max-count=1) >> versions.pri
	@cp versions.pri versions.mak
	@grep ^XIVOVER common.pri >> versions.mak
	@${ECHO} -n "version (after update) : " && $(MAKE) -s displayversions

displayversions:
	@${ECHO} ${XIVOVER}-${GIT_DATE}-${GIT_HASH}

# to be executed under a mingw/dos-like terminal
win32-%:
	@cd $* && ${QMAKE} && $(MAKE) release

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
	@$(MAKE) macos-baselib
	@$(MAKE) macos-plugins
	@$(MAKE) macos-xivoclient

macos-%:
	@cd $* && ${QMAKE} -macx && $(MAKE)

packmacos:
	@./cross/macos_pack.sh

#macos-%:
#	@strip $*/$*.app/Contents/MacOS/$*
#	@${UPXRUN} $*/$*.app/Contents/MacOS/$*
#	@mkdir -p $*/$*.app/Contents/Resources/French.lproj
#	@hdiutil create $*-${_XIVOVER_}-${_SVNVER_}.dmg -srcfolder $*/$*.app -format UDZO



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

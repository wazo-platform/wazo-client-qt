QMAKE?=qmake
ECHO?=/bin/echo
DOXYGEN?=doxygen
XC_VERSION?=1.2
VERSIONS_FILE=versions.mak

# WIN32 targets only
MAKENSIS=/cygdrive/c/Program\ Files/NSIS/makensis.exe
UPXWIN?=/cygdrive/c/upx303w/upx.exe

# Versions Definitions
-include ${VERSIONS_FILE}

default: all

help:
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

displayversions:
	@${ECHO} ${XIVOVER}-${GIT_DATE}-${GIT_HASH}

# to be executed under a bash/cygwin-like terminal
versions:
	
	${ECHO} -n "version (before update) : " && $(MAKE) -s displayversions

	rm -f ${VERSIONS_FILE}
	
	${ECHO} -n "XC_UNAME = " > ${VERSIONS_FILE}
	uname -s | sed "s/Linux/linux/;s/CYGWIN.*/win32/;s/Darwin/macos/" >> ${VERSIONS_FILE}
	
	${ECHO} -n "GIT_HASH = " >> ${VERSIONS_FILE}
	git log -1 --pretty=%h $(git rev-list HEAD --max-count=1) >> ${VERSIONS_FILE}
	
	${ECHO} -n "GIT_DATE = " >> ${VERSIONS_FILE}
	git log -1 --pretty=%ct $(git rev-list HEAD --max-count=1) >> ${VERSIONS_FILE}
	
	${ECHO} XIVOVER = ${XC_VERSION} >> ${VERSIONS_FILE}
	
	${ECHO} -n "GIT_DIR = " >> ${VERSIONS_FILE}
	git rev-parse --show-toplevel >> ${VERSIONS_FILE}

	${ECHO} -n "DATEBUILD = " >> ${VERSIONS_FILE}
	LANG= date +%Y-%m-%dT%H:%M:%S >> ${VERSIONS_FILE}
	
	${ECHO} -n "version (after update) : " && $(MAKE) -s displayversions

all:
	@$(MAKE) -s versions
	@$(MAKE) os-all

tests:
	@$(MAKE) -s versions
	@$(MAKE) os-tests

os-%:
	# Example : os-all -> linux-all
	$(MAKE) ${XC_UNAME}-$*

distclean: clean-baselib clean-xivoclient clean-xletlib clean-xlets
	rm -f ${VERSIONS_FILE}
	rm -rf xivoclient/obj xivoclient/bin
	rm -rf bin

clean-baselib:
	@$(MAKE) -C baselib distclean || true
	rm -rf baselib/obj baselib/bin

clean-%:
	@$(MAKE) -C xivoclient -f Makefile_$* distclean || true

# LINUX targets
# kind of dirtier than "all-linux: versions linux-xivoclient"
# but allows the 'include versions.mak' to be reloaded once it has been set
linux-all:
	@$(MAKE) linux-baselib
	@$(MAKE) linux-xletlib
	@$(MAKE) linux-xivoclient
	@$(MAKE) linux-xlets

linux-tests:
	@cd baselib && ${QMAKE} -o Makefile_tests tests.pro && $(MAKE) ${XC_JOPT} -f Makefile_tests
	@mkdir -p bin
	@cp -rd baselib/bin/* bin

linux-baselib:
	@cd baselib && ${QMAKE} -o Makefile_baselib baselib.pro && $(MAKE) ${XC_JOPT} -f Makefile_baselib
	@mkdir -p bin
	@cp -rd baselib/bin/* bin

linux-%:
	@cd xivoclient && ${QMAKE} -o Makefile_$* $*.pro && $(MAKE) ${XC_JOPT} -f Makefile_$*

# WIN32 targets

win32-all:
	@echo "   (under Qt prompt) :"
	@pwd | sed "s#/#\\\\#g;s#^#      cd C:\\\\cygwin#"
	@echo "      $(MAKE) win32-baselib win32-gui win32-xivoclient win32-plugins"
	@echo "   (back here under Cygwin) :"
	@echo "      $(MAKE) win32pack-xivoclient"

# to be executed under a mingw/dos-like terminal
win32-%:
	@cd src/$* && ${QMAKE} && $(MAKE) release

# to be executed under a bash/cygwin-like terminal
win32pack-%:
	@${UPXWIN} src/$*/release/$*.exe || true
	@rm -f delivery/settings.nsh
	@${ECHO} "!define XIVOVER ${_XIVOVER_}" >> delivery/settings.nsh
	@${ECHO} "!define SVNVER ${_SVNVER_}" >> delivery/settings.nsh
	@${MAKENSIS} delivery/$*.nsi
	@mv delivery/$*-setup-win32.exe $*-setup-${_XIVOVER_}-${_SVNVER_}-win32.exe

win32packdyn-%:
	@${UPXWIN} src/$*/release/$*.exe || true
	@rm -f delivery/settings.nsh
	@${ECHO} "!define XIVOVER ${_XIVOVER_}" >> delivery/settings.nsh
	@${ECHO} "!define SVNVER ${_SVNVER_}" >> delivery/settings.nsh
	@${ECHO} "!define QTBINPATH c:\Qt\4.5.2\bin" >> delivery/settings.nsh
	@${MAKENSIS} delivery/$*.nsi
	@mv delivery/$*-setup-win32.exe $*-setup-${_XIVOVER_}-${_SVNVER_}-win32.exe

# MACOS targets
# (man hdiutil) "-format UDBZ" works only >= 10.4, thus "-format UDZO"
# export UPXRUN=/Users/proformatique/upx-3.01-src/src/upx.out

macos-all:
	@$(MAKE) macos-baselib
	@$(MAKE) macos-gui
	@$(MAKE) macos-plugins
	@$(MAKE) macos-xivoclient

macos-%:
	@cd src/$* && ${QMAKE} -macx && $(MAKE) ${XC_JOPT}

packmacos:
	@./cross/macos_pack.sh

#macos-%:
#	@strip $*/$*.app/Contents/MacOS/$*
#	@${UPXRUN} $*/$*.app/Contents/MacOS/$*
#	@mkdir -p $*/$*.app/Contents/Resources/French.lproj
#	@hdiutil create $*-${_XIVOVER_}-${_SVNVER_}.dmg -srcfolder $*/$*.app -format UDZO



# DEBIAN targets

debian-all : debian-xivoclient

debian-%:
	@echo "## Building Debian Package for : $*"
	@echo "## Changelog will be updated with ${NAME} <${DEBEMAIL}>"
	cd src/$* && dch -v ${XIVOVER}~svn${SVNVER}-1 "New upstream release."
	rm -rf pf-xivo-cti-$**
	mkdir pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER}
	cp -rp src/$*/debian pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER}/debian
	cd src/$* && tar zcf ../pf-xivo-cti-$*_${XIVOVER}~svn${SVNVER}.orig.tar.gz $*
	cp src/$*/$* pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER}
	cd pf-xivo-cti-$*-${XIVOVER}~svn${SVNVER} && \
		DEB_AUTO_UPDATE_DEBIAN_CONTROL=yes fakeroot debian/rules clean && \
		dpkg-buildpackage -us -uc -tc -rfakeroot

# build doc

doc:
	${DOXYGEN} Doxyfile 

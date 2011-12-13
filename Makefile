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
	@echo "Enter make to build the XiVO Client."
	@echo
	@echo "Please see options on https://wiki.xivo.fr/index.php/XiVO_1.2-Skaro/CTI_XiVO_Client_Qt_Builder"
	@echo
	@echo "Thanks."
	@echo

# to be executed under a bash/cygwin-like terminal
# must be executed separately, since it generates a file included by make
# then, it shall not be included as dependencies of a target
versions premake:
	@echo -n 'version (before update) : '
	@$(MAKE) displayversions

	sh premake.sh ${VERSIONS_FILE} ${XC_VERSION}
	echo -n 'version (after update) : '

	@$(MAKE) displayversions

displayversions:
	echo ${XIVOVER}-${GIT_DATE}-${GIT_HASH}

all:
	@$(MAKE) -s versions
	@$(MAKE) os-all

tests:
	@$(MAKE) -s versions
	@$(MAKE) clean-tests
	@$(MAKE) os-tests DEBUG=yes
	@unit-tests/alltests.sh
	@cd baselib \
		&& ../unit-tests/tools/gcovr --xml -o ../unit-tests/cov-baselib.xml

functests:
	@$(MAKE) -s versions
	@$(MAKE) os-baselib
	@$(MAKE) os-xletlib
	@$(MAKE) os-xlets
	@$(MAKE) os-xivoclient-bin-dll
	ln -sf libpyxivoclient.so bin/_pyxivoclient.so
	@echo
	@echo "To use the Python module, do:"
	@echo "$$ export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:$$PWD/bin"
	@echo "$$ export PYTHONPATH=\$$PYTHONPATH:$$PWD/bin"
	@echo "$$ python"
	@echo ">>> import _pyxivoclient as xivoclient"

# Example : os-all -> linux-all
os-%:
	@$(MAKE) ${XC_UNAME}-$*

clean distclean: clean-tests clean-baselib clean-xivoclient clean-xletlib \
                 clean-xlets clean-functests
	rm -f ${VERSIONS_FILE}
	rm -rf xivoclient/obj xivoclient/bin
	rm -rf bin
	rm -f unit-tests/*.xml

clean-tests:
# We need to have a Makefile before cleaning
	-@cd baselib \
		&& ${QMAKE} -o Makefile_tests tests.pro \
		&& $(MAKE) distclean -f Makefile_tests

clean-baselib:
	-@$(MAKE) -C baselib distclean -f Makefile_baselib
	rm -rf baselib/obj baselib/bin

clean-%:
	-@$(MAKE) -C xivoclient -f Makefile_$* distclean

# LINUX targets

# We need the order, so no prerequisites
linux-all:
	@$(MAKE) linux-baselib
	@$(MAKE) linux-xletlib
	@$(MAKE) linux-xivoclient-bin-app
	@$(MAKE) linux-xlets

linux-tests:
	@cd baselib \
		&& ${QMAKE} -o Makefile_tests tests.pro \
		&& $(MAKE) ${XC_JOPT} -f Makefile_tests
	@mkdir -p bin
	@cp -rd baselib/bin/* bin

linux-baselib:
	@cd baselib \
		&& ${QMAKE} -o Makefile_baselib baselib.pro \
		&& $(MAKE) ${XC_JOPT} -f Makefile_baselib
	@mkdir -p bin
	@cp -rd baselib/bin/* bin

linux-%:
	@cd xivoclient \
		&& ${QMAKE} -o Makefile_$* $*.pro \
		&& $(MAKE) ${XC_JOPT} -f Makefile_$*

# WIN32 targets

# Get here when entering make.
# Print the instructions specific to Windows building
# "cygpath -wa ." gives the Windows path of pwd
# -w for Windows format
# -a for absolute path
win32-all:
	@echo "   (under Qt prompt) :"
	@echo "      cd $(shell cygpath -wa .)"
	@echo "      mingw32-make win32-build"
	@echo "   (back here under Cygwin) :"
	@echo "      make win32pack-xivoclient"

win32-build:
	@$(MAKE) win32-baselib win32-xletlib win32-xivoclient win32-xlets

# to be executed under a mingw/dos-like terminal
win32-baselib:
	@cd baselib \
		&& ${QMAKE} -o Makefile_baselib baselib.pro \
		&& $(MAKE) ${XC_JOPT} -f Makefile_baselib
# mkdir fails if bin already exists (no options)
	@-mkdir bin
# /Y silents xcopy when destination file already exists
	@xcopy /Y baselib\\bin\\*.dll bin

# mingw/dos-like terminal
win32-%:
	@cd xivoclient \
		&& ${QMAKE} -o Makefile_$* $*.pro \
		&& $(MAKE) ${XC_JOPT} -f Makefile_$*

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

docs:
	${DOXYGEN} Doxyfile

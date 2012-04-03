TEMPLATE = subdirs

# Generating versions.mak
include(xivoclient-all.pri)
VERSIONS_FILE = versions.mak
PREMAKE_COMMAND = ./premake.sh $${VERSIONS_FILE} $${XIVO_MAJOR_VERSION}

win32 {
    # Double quotes escapes && from cmd shell
    # OLDPWD is the current directory of cmd
    system($${CYGWIN_PATH}/bin/bash --login -c \"cd $OLDPWD && $${PREMAKE_COMMAND}\")
} else {
    system($${PREMAKE_COMMAND})
}

tests {
    SUBDIRS = tests

    tests.file = baselib/tests.pro
    tests.makefile = Makefile_tests

    unix:QMAKE_DISTCLEAN += -r bin/tests
    win32:QMAKE_DISTCLEAN += /s /f /q bin/tests && rd /s /q bin/tests
} else {
    SUBDIRS = baselib xletlib xlets xivoclient

    baselib.file = baselib/baselib.pro
    baselib.makefile = Makefile_baselib

    xletlib.file = xivoclient/xletlib.pro
    xletlib.makefile = Makefile_xletlib
    xletlib.depends = baselib

    xivoclient.file = xivoclient/xivoclient.pro
    xivoclient.makefile = Makefile_xivoclient
    xivoclient.depends = baselib xletlib

    xlets.file = xivoclient/xlets.pro
    xlets.makefile = Makefile_xlets
    xlets.depends = xletlib

    # Pack rule
    win32 {
        pack.target = pack
        pack.input = cross/resources/gpl.txt \
                     cross/resources/qt.conf \
                     cross/resources/xivo.bmp \
                     cross/resources/xivoclient.png \
                     cross/resources/xivoicon.ico \
                     cross/resources/xivoicon_un.ico \
                     cross/xc-packaging-windows.sh
        # Need $$ to escape $ from make
        # Need \$$ to escape $$ from qmake
        pack.commands = $${CYGWIN_PATH}/bin/bash --login -c \" \
            cd \$$OLDPWD &&\
            cross/xc-package-windows.sh \
                . \
                \$$\(which libstdc++-6.dll|xargs dirname\)/.. \
                \$$\(cygpath \$$QTDIR)\" &&
        pack.commands += \"$${NSIS_PATH}/makensis\" \
            $${CYGWIN_PATH}/tmp/xivoclient-win/installer.nsi &&
        pack.commands += $(COPY_FILE) \
           $${CYGWIN_PATH}\\tmp\\xivoclient-win\\xivoclient-inst.exe \
           %CD%\\xivoclient-$${XIVO_MAJOR_VERSION}.-x86.exe
    }
    unix {
        pack.target = pack
        pack.input = cross/resources/xivoclient.png \
                     cross/resources/xivoclient.desktop
        pack.commands = cross/xc-package-unix.sh
    }
    QMAKE_EXTRA_TARGETS += pack

    # Clean rules
    FILES_TO_CLEAN = versions.mak
    DIRS_TO_CLEAN = bin
    win32 {
        DIRS_TO_CLEAN += $${CYGWIN_PATH}\\tmp\\xivoclient-win
        QMAKE_DISTCLEAN += $${FILES_TO_CLEAN} /s /f /q $${DIRS_TO_CLEAN} && rd /s /q $${DIRS_TO_CLEAN}
    } else {
        QMAKE_DISTCLEAN += -r $${FILES_TO_CLEAN} $${DIRS_TO_CLEAN}
    }
}

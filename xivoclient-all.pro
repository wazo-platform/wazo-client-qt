TEMPLATE = subdirs

# Generating versions.mak
include(build-deps)
VERSIONS_FILE = versions.mak

system(bash premake.sh $${VERSIONS_FILE})

tests {
    SUBDIRS = baselib_tests xivoclient_tests

    baselib_tests.file = baselib/tests.pro
    baselib_tests.makefile = Makefile_tests

    xivoclient_tests.file = xivoclient/tests.pro
    xivoclient_tests.makefile = Makefile_tests

    QMAKE_DISTCLEAN += -r bin/tests
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
        pack.input = packaging/resources/gpl.txt \
                     packaging/resources/libeay32.dll \
                     packaging/resources/ssleay32.dll \
                     packaging/resources/qt.conf \
                     packaging/resources/wazo.bmp \
                     packaging/resources/wazoclient.png \
                     packaging/resources/wazoicon.ico \
                     packaging/xc-packaging-windows.sh
        pack.commands = packaging/xc-package-windows.sh
    }
    linux-g++ {
        pack.target = pack
        pack.input = packaging/resources/wazoclient.png \
                     packaging/resources/wazoclient.desktop
        pack.commands = packaging/xc-package-debian.sh
    }
    mac {
        pack.target = pack
        pack.input = packaging/resources/macos-info.plist
        pack.commands = packaging/xc-package-macos.sh .
    }
    QMAKE_EXTRA_TARGETS += pack

    # Translation rules
    pulltr.target = pulltr
    pulltr.commands = ./translations.sh pull
    pushtr.target = pushtr
    pushtr.commands = ./translations.sh push
    QMAKE_EXTRA_TARGETS += pulltr pushtr

    # Clean rules
    FILES_TO_CLEAN = versions.mak
    DIRS_TO_CLEAN = bin
    win32 {
        DIRS_TO_CLEAN += /tmp/xivoclient-win
    }
    QMAKE_DISTCLEAN += -r $${FILES_TO_CLEAN} $${DIRS_TO_CLEAN}
}

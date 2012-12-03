TEMPLATE = subdirs

# Generating versions.mak
include(xivoclient-all.pri)
VERSIONS_FILE = versions.mak

system(bash premake.sh $${VERSIONS_FILE})

tests {
    SUBDIRS = tests

    tests.file = baselib/tests.pro
    tests.makefile = Makefile_tests

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

    profiling {
        SUBDIRS = staticlib xivoclient

        staticlib.file = xivoclient/staticlib.pro
        staticlib.makefile = Makefile_staticlib

        xivoclient.depends = staticlib
    }

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
        pack.commands = cross/xc-package-windows.sh
    }
    linux-g++ {
        pack.target = pack
        pack.input = cross/resources/xivoclient.png \
                     cross/resources/xivoclient.desktop
        pack.commands = cross/xc-package-unix.sh
    }
    mac {
        pack.target = pack
        pack.input = cross/resources/macos-info.plist
        pack.commands = cross/xc-package-macos.sh \
            $${MAC_QT_PATH} \
            .
    }
    QMAKE_EXTRA_TARGETS += pack

    # Clean rules
    FILES_TO_CLEAN = versions.mak
    DIRS_TO_CLEAN = bin
    win32 {
        DIRS_TO_CLEAN += /tmp/xivoclient-win
    }
    QMAKE_DISTCLEAN += -r $${FILES_TO_CLEAN} $${DIRS_TO_CLEAN}
}

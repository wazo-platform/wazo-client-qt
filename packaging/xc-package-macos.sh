#!/usr/bin/env bash

if [ -r versions.mak ]
then
    source versions.mak
else
    echo "No versions.mak file found. Please run qmake."
    exit 1
fi

QT_PATH="$1"
XC_PATH="$2"
PACK_CONTENTS="${XC_PATH}/bin/xivoclient.app/Contents"

function link_baselib {
    lib="$1"
    install_name_tool -change \
        libxivoclient.1.dylib \
        @executable_path/libxivoclient.1.dylib \
        ${lib}
}

function link_xletlib {
    lib="$1"
    install_name_tool -change \
        libxivoclientxlets.1.dylib \
        @executable_path/libxivoclientxlets.1.dylib \
        ${lib}
}

function link_qt {
    lib="$1"
    install_name_tool -change \
        ${QT_PATH}/lib/QtGui.framework/Versions/4/QtGui \
        @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui \
        ${lib}
    install_name_tool -change \
        ${QT_PATH}/lib/QtNetwork.framework/Versions/4/QtNetwork \
        @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork \
        ${lib}
    install_name_tool -change \
        ${QT_PATH}/lib/QtCore.framework/Versions/4/QtCore \
        @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore \
        ${lib}
}

function link_baselib_from_app {
    app="$1"
    install_name_tool -change \
        '@executable_path/../Frameworks/libxivoclient.1.dylib' \
        '@executable_path/libxivoclient.1.dylib' \
        ${app}
}

function link_xletlib_from_app {
    app="$1"
    install_name_tool -change \
        '@executable_path/../Frameworks/libxivoclientxlets.1.dylib' \
        '@executable_path/libxivoclientxlets.1.dylib' \
        ${app}
}

function package {
    echo "Including Qt ..."
    macdeployqt ${XC_PATH}/bin/xivoclient.app

    echo "Including XiVO Client libs ..."
    cp -R ${XC_PATH}/bin/*.dylib ${PACK_CONTENTS}/MacOS

    echo "Making bundle depend on embedded Qt ..."

    link_baselib_from_app ${PACK_CONTENTS}/MacOS/xivoclient
    link_xletlib_from_app ${PACK_CONTENTS}/MacOS/xivoclient

    link_qt ${PACK_CONTENTS}/MacOS/libxivoclient.1.dylib

    link_baselib ${PACK_CONTENTS}/MacOS/libxivoclientxlets.1.dylib
    link_qt      ${PACK_CONTENTS}/MacOS/libxivoclientxlets.1.dylib

    for plugin in ${PACK_CONTENTS}/Resources/plugins/*.dylib ; do
        link_qt ${plugin}
        link_baselib ${plugin}
        link_xletlib ${plugin}
    done

    echo "Setting version ..."

    /usr/libexec/PlistBuddy -c "Add CFBundleShortVersionString String ${XC_VERSION}" bin/xivoclient.app/Contents/Info.plist

    echo "Creating .dmg package ..."
    hdiutil create xivoclient-${XC_VERSION}.dmg -srcfolder ${XC_PATH}/bin/xivoclient.app -format UDZO
}
package

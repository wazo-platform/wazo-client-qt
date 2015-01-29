#!/usr/bin/env bash

if [ -r versions.mak ]
then
    source versions.mak
else
    echo "No versions.mak file found. Please run qmake."
    exit 1
fi

source build-deps

function set_umask {
    umask 0022
}
set_umask

function get_infos {
    UNAME_ARCH="$(uname -m)" # Gets i?86 or x86_64
    case "$UNAME_ARCH" in
        *86)
            ARCH="i386"
            ;;
        *64)
            ARCH="amd64"
            ;;
    esac
}
get_infos

function package-content {
    cd "$GIT_DIR"

    PKGDIR=pkg
    PKGROOT=$PKGDIR/root
    RESOURCES_DIR=packaging/resources

    mkdir -p $PKGROOT/opt/xivoclient
    cp -r bin/* $PKGROOT/opt/xivoclient
    cp -P $DEBIAN_QT_PATH/lib/libQt5{Core,DBus,Gui,Network,Svg,Widgets,Xml}.so* $PKGROOT/opt/xivoclient
    cp -P $DEBIAN_QT_PATH/lib/libicu{i18n,uc,data}.so* $PKGROOT/opt/xivoclient
    cp -r $DEBIAN_QT_PATH/plugins/platforms $PKGROOT/opt/xivoclient

    mkdir -p $PKGROOT/usr/share/icons/hicolor/128x128/apps
    cp $RESOURCES_DIR/xivoclient.png $PKGROOT/usr/share/icons/hicolor/128x128/apps

    mkdir -p $PKGROOT/usr/share/applications/
    cp $RESOURCES_DIR/xivoclient.desktop $PKGROOT/usr/share/applications/
    sed -i s/xivoicon/xivoclient/ $PKGROOT/usr/share/applications/xivoclient.desktop

    mkdir -p $PKGROOT/usr/bin
    cp $RESOURCES_DIR/xivoclient.script $PKGROOT/usr/bin/xivoclient
    chmod 755 $PKGROOT/usr/bin/xivoclient
}
package-content

function package-control {
    mkdir -p pkg/control

    PKGSIZE=$(du -s $PKGROOT | cut -f 1)

    cat > pkg/control/control <<EOF
Package: xivoclient
Version: ${XC_VERSION}
Architecture: ${ARCH}
Maintainer: Avencall Maintainance Team <xivo-dev@lists.proformatique.com>
Installed-Size: ${PKGSIZE}
Depends: libc6 (>= 2.7-1), libgcc1 (>= 1:4.1.1-21), libstdc++6 (>= 4.1.1-21), libqtcore4 (>= 4.6.0), libqtgui4 (>= 4.6.0), libqt4-network (>= 4.6.0), libqt4-xml (>= 4.6.0)
Section: graphics
Priority: optional
Homepage: http://www.xivo.io/
Description: CTI client for XiVO
 XiVO CTI (Computer Telephony Integration) client is the graphical
 front-end to the XiVO CTI services.
EOF
    pushd $PKGROOT > /dev/null
    find * -type f | xargs md5sum > ../control/md5sums
    popd > /dev/null
}
package-control

function package {
    cd $PKGROOT
    tar zcf ../data.tar.gz --owner=0 --group=0 .
    cd ../control
    tar zcf ../control.tar.gz --owner=0 --group=0 .
    cd ..

    echo "2.0" > debian-binary

    PACKAGE_PATH="$GIT_DIR/xivoclient-${XC_VERSION}-${ARCH}.deb"
    rm -f "$PACKAGE_PATH"
    ar -r "$PACKAGE_PATH" debian-binary control.tar.gz data.tar.gz
}
package

function clean {
    cd ..
    rm -rf $PKGDIR
}
clean

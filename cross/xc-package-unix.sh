#!/usr/bin/env bash

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

    LAST_GIT_TAG="$(git tag -l | tail -n 1)"
    LAST_GIT_TAG_COMMIT=$(git rev-list $LAST_GIT_TAG --max-count=1)

    MAJOR_VERSION="${LAST_GIT_TAG:12:3}" # Gets 1.2, 1.3, etc.

    CURRENT_COMMIT=$(git rev-list HEAD --max-count=1)
    COMMIT_HASH=$(git log -1 --pretty=%h ${CURRENT_COMMIT})
    COMMIT_DATE=$(git log -1 --pretty=%ct ${CURRENT_COMMIT})

    if [ $CURRENT_COMMIT = $LAST_GIT_TAG_COMMIT ]
    then
        FULL_VERSION="${LAST_GIT_TAG:12}"
    else
        FULL_VERSION="${MAJOR_VERSION}-dev-${COMMIT_DATE}-${COMMIT_HASH}"
    fi
}
get_infos

function package-content {
    GITDIR="$(git rev-parse --show-toplevel)"
    cd "$GITDIR"

    PKGDIR=pkg
    PKGROOT=$PKGDIR/root

    mkdir -p $PKGROOT/opt/xivoclient
    cp -r bin/* $PKGROOT/opt/xivoclient

    mkdir -p $PKGROOT/usr/share/icons/
    cp cross/resources/xivoclient.png $PKGROOT/usr/share/icons/

    mkdir -p $PKGROOT/usr/share/applications/
    cp cross/resources/xivoclient.desktop $PKGROOT/usr/share/applications/
    sed -i s/xivoicon/xivoclient/ $PKGROOT/usr/share/applications/xivoclient.desktop

    mkdir -p $PKGROOT/usr/bin
    cat > $PKGROOT/usr/bin/xivoclient <<EOF
#!/bin/bash

DEBUG="no"

while getopts ":dh" opt; do
  case \$opt in
      d)
          DEBUG="yes"
          ;;
      h)
          echo "Usage : \$0 [-dh] [profile]"
          echo
          echo "-d : Enable debug output"
          echo "-h : Help"
          echo "profile : Configuration profile"
          echo
          exit 0
          ;;
      \?)
          echo "Invalid option: -\$OPTARG" >&2
          ;;
  esac
done

shift \$(( OPTIND-1 ))

cd /opt/xivoclient

if [ "\$DEBUG" = "yes" ]
then
    LD_LIBRARY_PATH=".:\$LD_LIBRARY_PATH" ./xivoclient \$@
else
    LD_LIBRARY_PATH=".:\$LD_LIBRARY_PATH" ./xivoclient \$@ >& /dev/null
fi
EOF
    chmod 755 $PKGROOT/usr/bin/xivoclient
}
package-content

function package-control {
    mkdir -p pkg/control

    PKGSIZE=$(du -s $PKGROOT | cut -f 1)

    cat > pkg/control/control <<EOF
Package: xivoclient
Version: ${FULL_VERSION}
Architecture: ${ARCH}
Maintainer: Proformatique Maintainance Team <technique@proformatique.com>
Installed-Size: ${PKGSIZE}
Depends: libc6 (>= 2.7-1), libgcc1 (>= 1:4.1.1-21), libstdc++6 (>= 4.1.1-21), libqtcore4 (>= 4.6.0), libqtgui4 (>= 4.6.0), libqt4-network (>= 4.6.0), libqt4-xml (>= 4.6.0)
Section: graphics
Priority: optional
Homepage: http://www.xivo.fr/
Description: CTI client for XiVO
 XiVO CTI (Computer Telephony Integration) client is the graphical
 front-end to the XiVO CTI services.
EOF
    find $PKGROOT/ -type f | xargs md5sum > pkg/control/md5sums
}
package-control

function package {
    cd $PKGROOT
    tar zcf ../data.tar.gz --owner=0 --group=0 .
    cd ../control
    tar zcf ../control.tar.gz --owner=0 --group=0 .
    cd ..

    echo "2.0" > debian-binary

    PACKAGE_PATH="$GITDIR/xivoclient-${FULL_VERSION}-${ARCH}.deb"
    rm -f "$PACKAGE_PATH"
    ar -r "$PACKAGE_PATH" debian-binary control.tar.gz data.tar.gz
}
package

function clean {
    cd ..
    rm -rf $PKGDIR
}
clean

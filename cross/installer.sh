#!/bin/bash
INSTALLER_PATH=$(dirname "$(pwd)/$0")
cd ${INSTALLER_PATH}

BUILT_DIR=$(pwd)/../../build/+latest-build+/
BENDER_RESOURCE=$(pwd)/../../installer/resource/

XIVO_VERSION=$(sed -n "s/XIVOVER[\t ]*=[\t ]*\([0-9.]*\)$/\1/gp" < ../common.pri )
SVN_VERSION=$(svn info|sed -n "s/Revision:[\t ]*//gp")
SVN_RESOURCE=${INSTALLER_PATH}/resources

# SRC  - path to dir
# DEST - outputed .deb
# $1 package_control
function do_a_deb_package 
{
  TMPDIR=$(date  +"/tmp/cticlient-deb-package-%s-%N")
  mkdir ${TMPDIR}
  cd ${TMPDIR}
  mkdir {root,control}
  # copy src dir in work, preserving right etc..
  cd root
  ( cd ${SRC} ;  tar -cf - .) | tar -xvf -

  PKGSIZE=$(du . | tail -n 1  | cut -f 1)
  cat > ../control/control <<!
Package: xivocticlient
Version: ${XIVO_VERSION}-${SVN_VERSION}
Architecture: ${ARCH}
Maintainer: Proformatique Maintainance Team <technique@proformatique.com>
Installed-Size: $PKGSIZE
Depends: libc6 (>= 2.7-1), libgcc1 (>= 1:4.1.1-21), libstdc++6 (>= 4.1.1-21)
Section: graphics
Priority: optional
Homepage: http://www.xivo.fr/
Description: CTI client for XiVO
 XiVO CTI (Computer Telephony Integration) client is the graphical
 front-end to the XiVO CTI services.
!
  find ./ -type f -name "*" | sed "s/.*/md5sum &/g" | sh | sed "s/\([^ ]*\)[ \t]*\(.*\)/\1  \2/g" > ../control/md5sums

  tar -zcf ../data.tar.gz .
  cd ../control ; tar zcf ../control.tar.gz .

  cd .. ; mkdir pkg ; cd pkg ; mv ../*.gz .
  echo "2.0" > debian-binary

  rm -f ${DST}
  ar -r ${DST} debian-binary control.tar.gz data.tar.gz
  
  rm -rf ${TMPDIR}
}


PACKAGE_WORK_DIR=$(date +"/tmp/package-workdir-%s-%N")
mkdir -p ${PACKAGE_WORK_DIR}/work

cd ${PACKAGE_WORK_DIR}
( cd ${BUILT_DIR}/$1 ; tar -cf - . ) | (cd work ; tar -xf -)

. ${INSTALLER_PATH}/installer_config/$1

rm -rf ${PACKAGE_WORK_DIR}

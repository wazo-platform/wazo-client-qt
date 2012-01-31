#!/bin/bash
TARGET_SVN_VERSION=1.6
cd $(dirname "$(pwd)/$0")

# Set up a temporary dir on a remote host to build the xivoclient
# TARGET_HOST 
# TARGET_SVN_VERSION

function set_up_build_dir
{
if [ "${TARGET_SVN_VERSION}" ] ; then
  ( cd ../ ; change-svn-wc-format.py . ${TARGET_SVN_VERSION} )
fi

TEMPDIR=/tmp/xivoclient-build-$(date +%s)
( cat <<!
  mkdir -p ${TEMPDIR} ; cd ${TEMPDIR}
  cat - | tar -zxvf -
!
tar -zcf - ../../workdir ) | ssh -T ${TARGET_HOST}
}

function make_the_build
{
  ( echo "cd ${TEMPDIR}/workdir/" ; cat - ) | ssh -T ${TARGET_HOST}
}

# reduce things fetched by defining $1
function fetch_built_files
{
  cd ..
  rm -rf bin
  echo "cd ${TEMPDIR}/workdir/ ; tar -zcf - bin/$1" | ssh -T ${TARGET_HOST} | \
  tar -zxvf -
}

function remove_build_dir
{
  ssh ${TARGET_HOST} "rm -rf ${TEMPDIR}"
}

. build_config/$1

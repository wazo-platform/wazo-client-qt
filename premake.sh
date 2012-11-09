#!/usr/bin/env bash

# This script might be executed by an UNIX shell OR by Cygwin.
# If you use advanced shell features, please check that Cygwin accepts them
# as well.

if [ $# -lt 1 ] ; then
    echo "Usage : $0 <versions_file>"
    echo "Example : $0 versions.mak"
    exit
fi

VERSIONS_FILE=$1

XC_UNAME="$(uname -s | sed 's/Linux/linux/;s/CYGWIN.*/win32/;s/Darwin/macos/')"

GIT_DIR=$(git rev-parse --show-toplevel)
if [ "$XC_UNAME" = "win32" ] ; then
    # Convert to Windows format, as the file will be included in .pro files
    GIT_DIR="$(cygpath -m $GIT_DIR)"
fi
CURRENT_COMMIT="$(git rev-list HEAD --max-count=1)"
COMMIT_HASH="$(git log -1 --pretty=%h $CURRENT_COMMIT)"
COMMIT_DATE="$(git log -1 --pretty=%ct $CURRENT_COMMIT)"
GIT_DESCRIBE="$(git describe)"
XC_VERSION="${GIT_DESCRIBE:12}" # Strip "xivo-client-" from tag name
DATEBUILD="$(LANG= date +%Y-%m-%dT%H:%M:%S)"


cat > ${VERSIONS_FILE} <<EOF
# Precompilation infos
GIT_DIR=$GIT_DIR

# Compilation infos
XC_UNAME=$XC_UNAME
GIT_HASH=$COMMIT_HASH
GIT_DATE=$COMMIT_DATE
XC_VERSION=$XC_VERSION
DATEBUILD=$DATEBUILD
EOF

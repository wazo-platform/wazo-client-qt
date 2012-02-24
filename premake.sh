#!/usr/bin/env bash

# This script might be executed by an UNIX shell OR by Cygwin.
# If you use advanced shell features, please check that Cygwin accepts them
# as well.

if [ ! $# -eq 2 ] ; then
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
LAST_GIT_TAG="$(git tag -l | tail -n 1)"
LAST_GIT_TAG_COMMIT="$(git rev-list $LAST_GIT_TAG --max-count=1)"
LAST_GIT_TAG_VERSION="${LAST_GIT_TAG:12}" # Gets 1.2.1, 1.2.2, ...
if [ "$CURRENT_COMMIT" = "$LAST_GIT_TAG_COMMIT" ]
then
    XC_VERSION="$LAST_GIT_TAG_VERSION"
else
    XC_VERSION="dev-$COMMIT_DATE-$COMMIT_HASH"
fi
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

# Packaging infos
LAST_GIT_TAG=$LAST_GIT_TAG
LAST_GIT_TAG_COMMIT=$LAST_GIT_TAG_COMMIT
LAST_GIT_TAG_VERSION=$LAST_GIT_TAG_VERSION
EOF

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

GIT_DIR=$(git rev-parse --show-toplevel)
CURRENT_COMMIT="$(git rev-list HEAD --max-count=1)"
COMMIT_HASH="$(git log -1 --pretty=%h $CURRENT_COMMIT)"
COMMIT_DATE="$(git log -1 --pretty=%ct $CURRENT_COMMIT)"
GIT_DESCRIBE="$(git describe)"
XC_VERSION="${GIT_DESCRIBE:12}" # Strip "wazo-client-" from tag name
DATEBUILD="$(LANG= date +%Y-%m-%dT%H:%M:%S)"


cat > ${VERSIONS_FILE} <<EOF
# Precompilation infos
GIT_DIR=$GIT_DIR

# Compilation infos
GIT_HASH=$COMMIT_HASH
GIT_DATE=$COMMIT_DATE
XC_VERSION=$XC_VERSION
DATEBUILD=$DATEBUILD
EOF

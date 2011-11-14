# This script might be executed by an UNIX shell OR by Cygwin.
# If you use advanced shell features, please check that Cygwin accepts them
# as well.

if [ ! $# -eq 2 ] ; then
    echo "Usage : $0 <versions_file> <xivo_version>"
    echo "Example : $0 versions.mak 1.2"
    exit
fi

VERSIONS_FILE=$1
XC_VERSION=$2

if [ -f ${VERSIONS_FILE} ] ; then
    eval $(sed 's/ = /=/' ${VERSIONS_FILE})
fi

rm -f ${VERSIONS_FILE}

XC_UNAME=$(uname -s | sed "s/Linux/linux/;s/CYGWIN.*/win32/;s/Darwin/macos/")
echo -n "XC_UNAME = " >> ${VERSIONS_FILE}
echo ${XC_UNAME} >> ${VERSIONS_FILE}

echo -n "GIT_HASH = " >> ${VERSIONS_FILE}
git log -1 --pretty=%h $(git rev-list HEAD --max-count=1) >> ${VERSIONS_FILE}

echo -n "GIT_DATE = " >> ${VERSIONS_FILE}
git log -1 --pretty=%ct $(git rev-list HEAD --max-count=1) >> ${VERSIONS_FILE}

echo "XIVOVER = ${XC_VERSION}" >> ${VERSIONS_FILE}

echo -n "GIT_DIR = " >> ${VERSIONS_FILE}
if [ ${XC_UNAME} = "win32" ] ; then
    # Prints Cygwin root path prefix
    echo -n $(cygpath -m /) >> ${VERSIONS_FILE}
fi
git rev-parse --show-toplevel >> ${VERSIONS_FILE}

echo -n "DATEBUILD = " >> ${VERSIONS_FILE}
LANG= date +%Y-%m-%dT%H:%M:%S >> ${VERSIONS_FILE}

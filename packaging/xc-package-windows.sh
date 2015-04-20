#!/usr/bin/env bash

set -e

function usage {
    echo $0 xivoclient_dir mingw_dir qt_dir
}

if [ -r versions.mak ]
then
    source versions.mak
else
    echo "No versions.mak file found. Please run qmake."
    exit 1
fi

source build-deps

XC_DIR=.
MINGW_DIR=${WIN_MINGW_PATH}
QT_DIR=${WIN_QT_PATH}
RES_DIR=${XC_DIR}/packaging/resources
TEMPLATES_DIR=${XC_DIR}/packaging/templates
DEST_DIR=/tmp/xivoclient-win

rm -rf $DEST_DIR
mkdir $DEST_DIR

# Resources
cp -r ${RES_DIR}/{gpl.txt,xivoicon.ico,xivoicon_un.ico,xivo.bmp} $DEST_DIR

# Qt libs
rm -rf $DEST_DIR/qtlibs
mkdir $DEST_DIR/qtlibs
cp $MINGW_DIR/bin/\
{libstdc++-6.dll,\
libgcc_s_dw2-1.dll} $DEST_DIR/qtlibs
cp $QT_DIR/bin/\
{Qt5Core.dll,\
Qt5Gui.dll,\
Qt5Multimedia.dll,\
Qt5Network.dll,\
Qt5Sql.dll,\
Qt5Svg.dll,\
Qt5Widgets.dll,\
Qt5Xml.dll,\
Qt5XmlPatterns.dll,\
libwinpthread-1.dll,\
icuin*.dll,\
icuuc*.dll,\
icudt*.dll} $DEST_DIR/qtlibs
rm -rf $DEST_DIR/qtlibs/{imageformats,platforms,sqldrivers}
mkdir $DEST_DIR/qtlibs/{imageformats,platforms,sqldrivers}
cp -r $QT_DIR/plugins/imageformats/*.dll $DEST_DIR/qtlibs/imageformats
cp -r $QT_DIR/plugins/platforms/*.dll $DEST_DIR/qtlibs/platforms
cp -r $QT_DIR/plugins/sqldrivers/*.dll $DEST_DIR/qtlibs/sqldrivers

# Qt translations
rm -rf $DEST_DIR/qtlibs/Qt
mkdir $DEST_DIR/qtlibs/Qt
cp -r $QT_DIR/translations $DEST_DIR/qtlibs/Qt
cp ${RES_DIR}/qt.conf $DEST_DIR/qtlibs

# OpenSSL libraries
cp ${RES_DIR}/libeay32.dll $DEST_DIR/qtlibs
cp ${RES_DIR}/ssleay32.dll $DEST_DIR/qtlibs

# XiVO Client binaries
rm -rf $DEST_DIR/xivoclient
mkdir $DEST_DIR/xivoclient
cp -r $XC_DIR/bin/*.dll $XC_DIR/bin/*.exe $DEST_DIR/xivoclient
mkdir $DEST_DIR/xivoclient/plugins
cp -r $XC_DIR/bin/plugins/*.dll $DEST_DIR/xivoclient/plugins

# Reset permissions in case they are broken by ACL
find $DEST_DIR -type d -exec chmod 0755 {} \;
find $DEST_DIR -type f -exec chmod 0644 {} \;

cp "${TEMPLATES_DIR}"/windows-installer.nsi "${DEST_DIR}"/installer.nsi
"${WIN_NSIS_PATH}"/makensis \
    /DXC_VERSION="${XC_VERSION}" \
    /DGIT_HASH="${GIT_HASH}" \
    /DGIT_DATE="${GIT_DATE}" \
    $(cygpath -w "${DEST_DIR}"/installer.nsi)

mv "${DEST_DIR}"/xivoclient-*.exe "${XC_DIR}"

rm -rf "${DEST_DIR}"

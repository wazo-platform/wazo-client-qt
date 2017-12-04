#!/usr/bin/env bash

set -e

if [ -r versions.mak ]
then
    source versions.mak
else
    echo "No versions.mak file found. Please run qmake."
    exit 1
fi

XC_PATH="$(cd "$1" && pwd)"

BUNDLE_NAME="wazoclient.app"
DMG_NAME="wazoclient.dmg"

BUILD_PATH="${XC_PATH}/dmg"
BUNDLE_PATH="${BUILD_PATH}/${BUNDLE_NAME}"


function clean_build() {
	rm -rf ${BUILD_PATH}
	rm -f ${XC_PATH}/*.dmg
}

function prepare_build() {
	mkdir -p ${BUILD_PATH}
	cp -r ${XC_PATH}/bin/* ${BUILD_PATH}
}

function set_version() {
    /usr/libexec/PlistBuddy -c "Add CFBundleShortVersionString String ${XC_VERSION}" ${BUNDLE_PATH}/Contents/Info.plist
}

function build_package() {
	macdeployqt ${BUNDLE_PATH} -dmg -libpath=${BUILD_PATH}
	mv ${BUILD_PATH}/${DMG_NAME} ${XC_PATH}/wazoclient-${XC_VERSION}.dmg
}

function package {
	echo "Cleaning build ..."
	clean_build
	echo "Preparing build ..."
	prepare_build
	echo "Setting version ..."
	set_version
	echo "Building .dmg package ..."
	build_package
}

package

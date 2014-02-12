#!/bin/bash

msg() {
  echo "" >&2
  echo -e "   \033[1;34m->\033[1;0m \033[1;1m${1}\033[1;0m" >&2
  echo "" >&2
}

error() {
  echo -e "\033[1;31m==> ERROR:\033[1;0m \033[1;1m$1\033[1;0m" >&2
}

if [ "$0" != "./scripts/release_source_packages.sh" ]; then
	if [ "$0" != "scripts/release_source_packages.sh" ]; then
		error "Run this script from project root directory using command './scripts/release_source_packages.sh'"
		exit
	fi
fi

DIR=$PWD
BUILD_DIR=$DIR/buildtmp
RELEASE_DIR=$DIR/release
BUILDSCRIPT_DIRS=("$DIR/scripts/ArchLinux" "$DIR/scripts/ArchLinux_mingw-w64")

mkdir -p $RELEASE_DIR
rm -rf $BUILDDIR

for buildscript_dir in "${BUILDSCRIPT_DIRS[@]}"; do
	msg "Building $buildscript_dir/PKGBUILD source package ..."
	mkdir -p $BUILD_DIR
	cd $BUILD_DIR
	
	cp $buildscript_dir/* ./
	sed -i -e "s|pkgver=.*$|pkgver=$(cat ${DIR}/VERSION)|" PKGBUILD
	makepkg -S
	cp *.src.tar.gz $RELEASE_DIR/
	rm -rf $BUILD_DIR
done

msg "Built all source packages."

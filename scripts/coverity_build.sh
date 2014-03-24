#!/bin/bash

msg() {
  echo ""
  echo -e "   \033[1;34m->\033[1;0m \033[1;1m${1}\033[1;0m" >&2
}

error() {
  echo -e "\033[1;31m==> ERROR:\033[1;0m \033[1;1m$1\033[1;0m" >&2
}

if [ "$0" != "./scripts/coverity_build.sh" ]; then
    if [ "$0" != "scripts/coverity_build.sh" ]; then
        error "Run this script from project root directory using command './scripts/coverity_build.sh'"
        exit
    fi
fi

msg "Coverity build requested..."
CURDIR=$PWD
DIR=coverity-build
RESULT=result
TARBALL=myproject.tgz
TARBALL_BASE=$(basename $TARBALL)

msg "Creating build directory..."
rm -rf $DIR
if [ "$?" != "0" ]; then error "rm failed! Aborting."; exit; fi
mkdir $DIR
if [ "$?" != "0" ]; then error "mkdir failed! Aborting."; exit; fi
cd $DIR

msg "Configure build files..."
cmake ../ -DCOMPATIBLE=1
if [ "$?" != "0" ]; then error "cmake failed! Aborting."; exit; fi

msg "Build project..."
cov-build --dir cov-int make -j2 | tee $RESULT
if [ "$?" != "0" ]; then error "make failed! Aborting."; exit; fi
grep -e "(100%)" $RESULT >/dev/null
if [ "$?" != "0" ]; then
    error "Not all units are ready for analysis. Aborting."
    exit
fi

msg "Packaging files..."
tar czvf $TARBALL cov-int
if [ "$?" != "0" ]; then error "tar failed! Aborting."; exit; fi
mv $TARBALL $CURDIR/$TARBALL_BASE
if [ "$?" != "0" ]; then error "mv failed! Aborting."; exit; fi

msg "Cleaning up files..."
cd $CURDIR
rm -rf $DIR
if [ "$?" != "0" ]; then error "rm failed! Aborting."; exit; fi

msg "Tarball is located under '$CURDIR/$TARBALL_BASE'"

msg "Uploading tarball..."
curl --noproxy '*' --form project=SphereSim --form token=TITPuELE6wnJfQAT19Hbfw --form email=max.mail@dameweb.de \
    --form file=@"$CURDIR/$TARBALL_BASE" --form version="$(cat $CURDIR/VERSION)" \
    --form description="$(cat $CURDIR/VERSION) build" http://scan5.coverity.com/cgi-bin/upload.py
if [ "$?" != "0" ]; then error "curl failed! Aborting."; exit; fi

msg "Finished! Tarball was uploaded."

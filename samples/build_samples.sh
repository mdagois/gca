#!/usr/bin/bash

FULL_PATH=$(realpath "$0")
BASE_DIR=$(dirname $FULL_PATH)

SAMPLE_DIR=./
ROMS_DIR=$(realpath ../prebuilt/roms)

echo ROMS: $ROMS_DIR

rm -rf $ROMS_DIR
mkdir $ROMS_DIR

pushd $BASE_DIR/$SAMPLE_DIR

SUCCESS=0
ERROR=0

for sample_dir in *
do
	pushd "$sample_dir"
	echo Building [$sample_dir]...
	sh build.sh
	if [ $? -eq 0 ]; then
		echo SUCCESS
		SUCCESS=`expr $SUCCESS + 1`
		cp *.gb* $ROMS_DIR
	else
		echo ERROR
		ERROR=`expr $ERROR + 1`
	fi
	popd
	echo " "
done

popd

echo SUCCESS: $SUCCESS, ERROR: $ERROR
exit 0


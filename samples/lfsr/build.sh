#!/usr/bin/sh

FULL_PATH=$(realpath "$0")
BASE_DIR=$(dirname $FULL_PATH)
ROM_NAME=$(basename $BASE_DIR)

pushd $BASE_DIR

rgbasm -Werror -Weverything -Hl -o main.o main.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -Hl -o sample.o sample.rgbasm
[ $? -eq 0 ] || exit 1
rgblink --dmg --tiny --map $ROM_NAME.map --sym $ROM_NAME.sym -o $ROM_NAME.gb main.o sample.o
[ $? -eq 0 ] || exit 1
rgbfix --title game --pad-value 0 --validate $ROM_NAME.gb
[ $? -eq 0 ] || exit 1

popd

exit 0


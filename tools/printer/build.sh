#!/usr/bin/sh

FULL_PATH=$(realpath "$0")
BASE_DIR=$(dirname $FULL_PATH)
ROM_NAME=$(basename $BASE_DIR)

pushd $BASE_DIR

rgbasm -Werror -Weverything -o main.o main.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -o data.o data.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -o memory.o memory.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -o printer.o printer.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -o interrupt.o interrupt.rgbasm
[ $? -eq 0 ] || exit 1
rgblink --dmg --tiny --map $ROM_NAME.map --sym $ROM_NAME.sym -o $ROM_NAME.gb main.o data.o interrupt.o memory.o printer.o
[ $? -eq 0 ] || exit 1
rgbfix --title game --pad-value 0 --validate $ROM_NAME.gb
[ $? -eq 0 ] || exit 1

popd

exit 0


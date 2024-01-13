#!/usr/bin/sh

FULL_PATH=$(realpath "$0")
BASE_DIR=$(dirname $FULL_PATH)
ROM_NAME=$(basename $BASE_DIR)

pushd $BASE_DIR

rgbasm -Werror -Weverything -o main.o main.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -o channel_data.o channel_data.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -o shared.o shared.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -o ch1.o ch1.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -o ch2.o ch2.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -o ch3.o ch3.rgbasm
[ $? -eq 0 ] || exit 1
rgbasm -Werror -Weverything -o ch4.o ch4.rgbasm
[ $? -eq 0 ] || exit 1
rgblink --dmg --tiny --map $ROM_NAME.map --sym $ROM_NAME.sym -o $ROM_NAME.gb main.o channel_data.o shared.o ch1.o ch2.o ch3.o ch4.o
[ $? -eq 0 ] || exit 1
rgbfix --title game --pad-value 0 --validate $ROM_NAME.gb
[ $? -eq 0 ] || exit 1

popd

exit 0


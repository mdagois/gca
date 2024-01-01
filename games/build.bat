echo off
pushd "%~dp0"

mkdir build
pushd build
mkdir .temp
popd

rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/gamestate_play.rgbasm.o minesweep/gamestate_play.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/cursor.rgbasm.o minesweep/cursor.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/board.rgbasm.o minesweep/board.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/interrupts.rgbasm.o minesweep/interrupts.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/gamestate_lose.rgbasm.o minesweep/gamestate_lose.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/gamestate_uncover.rgbasm.o minesweep/gamestate_uncover.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/gamestate_waitfornewgame.rgbasm.o minesweep/gamestate_waitfornewgame.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/sound.rgbasm.o minesweep/sound.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/gamestate_win.rgbasm.o minesweep/gamestate_win.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/gamestate_showresults.rgbasm.o minesweep/gamestate_showresults.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/gamestate.rgbasm.o minesweep/gamestate.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/data.rgbasm.o minesweep/data.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/minesweep.rgbasm.o minesweep/minesweep.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/counter.rgbasm.o minesweep/counter.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/gamestate_flag.rgbasm.o minesweep/gamestate_flag.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/memory.rgbasm.o minesweep/memory.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/window.rgbasm.o minesweep/window.rgbasm
rgbasm -I. -I./common/include -Iminesweep/ -Werror -Weverything -DBUILD_RELEASE --output build/.temp/gamestate_newgame.rgbasm.o minesweep/gamestate_newgame.rgbasm
rgblink --pad 199 --dmg --tiny --wramx --map build/minesweep.map --sym build/minesweep.sym --output build/minesweep.gb build/.temp/gamestate_play.rgbasm.o build/.temp/cursor.rgbasm.o build/.temp/board.rgbasm.o build/.temp/interrupts.rgbasm.o build/.temp/gamestate_lose.rgbasm.o build/.temp/gamestate_uncover.rgbasm.o build/.temp/gamestate_waitfornewgame.rgbasm.o build/.temp/sound.rgbasm.o build/.temp/gamestate_win.rgbasm.o build/.temp/gamestate_showresults.rgbasm.o build/.temp/gamestate.rgbasm.o build/.temp/data.rgbasm.o build/.temp/minesweep.rgbasm.o build/.temp/counter.rgbasm.o build/.temp/gamestate_flag.rgbasm.o build/.temp/memory.rgbasm.o build/.temp/window.rgbasm.o build/.temp/gamestate_newgame.rgbasm.o && rgbfix --pad-value 199 --validate --title minesweep build/minesweep.gb


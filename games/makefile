########################################
# global
########################################

projects = minesweep
configurations = debug release
default_target = minesweep_release

source_directory = .
build_directory = build

debug_description = build with debug features and extra checks enabled
release_description = deliverable build

compile_options = -Werror -Weverything -Hl -I./common/include
debug_compile_options = -DBUILD_DEBUG
release_compile_options = -DBUILD_RELEASE

link_options = --pad 199

fix_options = --pad-value 199 --validate

launch_options = --watch

gfxconv = ../bin/gfxconv

########################################
# minesweep
########################################

minesweep_description = a simple minesweep game

minesweep_source_sub_directory = minesweep
minesweep_sources = \
	board.rgbasm \
	counter.rgbasm \
	cursor.rgbasm \
	data.rgbasm \
	gamestate.rgbasm \
	gamestate_flag.rgbasm \
	gamestate_lose.rgbasm \
	gamestate_newgame.rgbasm \
	gamestate_play.rgbasm \
	gamestate_showresults.rgbasm \
	gamestate_uncover.rgbasm \
	gamestate_waitfornewgame.rgbasm \
	gamestate_win.rgbasm \
	interrupts.rgbasm \
	memory.rgbasm \
	minesweep.rgbasm \
	window.rgbasm
minesweep_link_options = --dmg --tiny --wramx
minesweep_debug_compile_options = -DSHOW_MINES

########################################
# extra rules
########################################

%.chr: %.png
	$(gfxconv) $<

########################################

include common/gbbs.mk


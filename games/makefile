projects = minesweep
configurations = debug release
default_target = minesweep_release

source_directory = .
build_directory = build

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
	gamestate_uncover.rgbasm \
	gamestate_waitfornewgame.rgbasm \
	gamestate_win.rgbasm \
	interrupts.rgbasm \
	memory.rgbasm \
	minesweep.rgbasm \
	window.rgbasm
minesweep_link_options = --dmg --tiny --wramx
minesweep_debug_compile_options = -DSHOW_MINES

####################

minesweep_targets: minesweep_resources

minesweep_png_list = tiles.png
minesweep_resources_list =  $(addprefix $(minesweep_source_sub_directory)/res/,$(minesweep_png_list:.png=.chr))
minesweep_resources: $(minesweep_resources_list)
%.chr: %.png
	$(gfxconv) $<

.PHONY: minesweep_resources

########################################

include common/gbbs.mk


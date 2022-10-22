projects = minesweep
configurations = debug release
default_target = minesweep_release

source_directory = .
build_directory = build

compile_options = -Werror -Weverything -Hl -I./common/include
debug_compile_options = -DBUILD_DEBUG -DSHOW_MINES
release_compile_options = -DBUILD_RELEASE

fix_options = --pad-value 0 --validate

launch_options = --watch

gfxconv = ../bin/gfxconv

########################################
# minesweep
########################################

minesweep_source_sub_directory = minesweep
minesweep_sources = \
	counter.rgbasm \
	cursor.rgbasm \
	data.rgbasm \
	interrupts.rgbasm \
	main.rgbasm \
	memory.rgbasm \
	minesweep.rgbasm \
	window.rgbasm
minesweep_link_options = --dmg --tiny --wramx

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


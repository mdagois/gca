projects = minesweep
configurations = debug release
default_target = minesweep_release

source_directory = .
build_directory = build

compile_options = -Werror -Weverything -Hl -I./common/include
debug_compile_options = -DBUILD_DEBUG
release_compile_options = -DBUILD_RELEASE

fix_options = --pad-value 0 --validate

launch_options = --watch

########################################
# minesweep
########################################

minesweep_source_sub_directory = minesweeper/src
minesweep_sources = \
	game.rgbasm \
	main.rgbasm

minesweep_link_options = --dmg --tiny --wramx

minesweep_release: $(minesweep_source_sub_directory)/tiles.chr

$(minesweep_source_sub_directory)/tiles.chr: $(minesweep_source_sub_directory)/tiles.png
	@../bin/gfxconv $<

$(minesweep_source_sub_directory)/tiles.png: $(minesweep_source_sub_directory)/../art/tiles.ase
	@aseprite -b $< --save-as $@

.INTERMEDIATE: $(minesweep_source_sub_directory)/tiles.png

########################################

include common/gbbs.mk


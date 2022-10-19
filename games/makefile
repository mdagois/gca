projects = minesweep
configurations = debug release
default_target = minesweep_release

source_directory = .
build_directory = build

debug_compile_options = -DBUILD_DEBUG
release_compile_options = -DBUILD_RELEASE

########################################
# minesweep
########################################

minesweep_source_sub_directory = minesweeper/src
minesweep_sources = \
	game.rgbasm \
	main.rgbasm

minesweep_compile_options = -Werror -Weverything -Hl
minesweep_link_options = --dmg --tiny --wramx
minesweep_fix_options = --pad-value 0 --validate
minesweep_launch_options = --watch

########################################

include gbbs.mk


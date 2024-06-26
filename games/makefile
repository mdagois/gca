########################################
# global
########################################

projects = minesweep pong
configurations = debug release
default_target = all

build_directory = build

debug_description = build with debug features and extra checks enabled
release_description = deliverable build

compile_options = -Werror -Weverything -I./common/include
debug_compile_options = -DBUILD_DEBUG
release_compile_options = -DBUILD_RELEASE

link_options = --pad 199

fix_options = --pad-value 199 --validate

launch_options = --watch

gfxconv = ../bin/gfxconv

########################################
# minesweep
########################################

minesweep_description = a 16x16 minesweep game
minesweep_sources = $(wildcard minesweep/*.rgbasm)
minesweep_link_options = --dmg --tiny --wramx
minesweep_debug_compile_options = -DSHOW_MINES

########################################
# pong
########################################

pong_description = a brick breaker game
pong_sources = $(wildcard pong/*.rgbasm)
pong_link_options = --dmg --tiny --wramx
pong_debug_compile_options = -DDEBUG_CONTROL_RIGHT_PAD

########################################
# resource rules
########################################

%.chr: %.png
	$(gfxconv) $<

resource_list = $(subst .png,.chr,$(foreach project,$(projects),$(wildcard $(project)/res/*.png)))
.PHONY: resource
resource: $(resource_list)

########################################

include ../tools/gbbs.mk


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

gfxconv = ../bin/gconv

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

resource_common_options = --tileset-divisions 8x8k --tile-removal none --skip-export-tileset-info --skip-export-palette

%.dmg-sp.chr: %.dmg-sp.png
	$(gfxconv) --hardware dmg-sp --tileset $< $(resource_common_options)

%.dmg-bg.chr: %.dmg-bg.png
	$(gfxconv) --hardware dmg-bg --tileset $< $(resource_common_options)

resource_list = $(subst .png,.chr,$(foreach project,$(projects),$(wildcard $(project)/res/*.png)))
.PHONY: resource
resources: $(resource_list)

clean_resources:
	@rm -f $(resource_list)

########################################

include ../tools/gbbs.mk


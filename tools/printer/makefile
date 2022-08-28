################################################################################
# README
#
# A simple makefile to build DMG and CGB ROMs.
#
# Steps
# 1. Edit the variables in the 'Parameters' section
# 2. In a console prompt, type any of the following
#    a. 'make romd' to build a debug version
#    b. 'make romr' to build a release version
# 3. Type 'make launch' to run the ROM
#
# Requirements
# - The 'rgbasm', 'rgblink' and 'rgbfix' executables are expected to be in your
#   path to build a ROM
# - The 'bgb' executable must be in your path to launch the ROM
################################################################################

################################################################################
# Parameters
#
# This section contains all the variables specific to a ROM.
################################################################################

# game name
# keep under 11 characters with no space
ROM_GAME_NAME=printer

# list of source file names (without extension)
ROM_FILES=\
	interrupt \
	main \
	memory \
	printer \
	protocol

# extension of source files
ROM_SRC_EXTENSION=rgbasm

# source directory
ROM_SRC_DIR=src/

# output directory for ROM, map and symbol files
ROM_DIR=rom/

# ROM extension
# should be 'gb' for DMG and 'gbc' for CGB
ROM_EXTENSION=gbc

# rgbasm extra options
ROM_COMPILE_OPTIONS=-Werror -Weverything

# rgblink extra options
ROM_LINK_OPTIONS=--tiny

# rgbfix extra options
ROM_FIX_OPTIONS=--color-only --pad-value 0 --validate

# options passed to bgb when launching the game
ROM_LAUNCH_OPTIONS=--listen 8765

################################################################################
# Inferences
#
# This section declares several variables used in the build rules.
# There is no need to edit this section.
################################################################################

ROM_OBJECTS=$(addprefix $(ROM_SRC_DIR),$(patsubst %,%.o,$(ROM_FILES)))

ROM_PATH=$(ROM_DIR)$(ROM_GAME_NAME)
ROM_FILE=$(ROM_PATH).$(ROM_EXTENSION)
ROM_MAP_FILE=$(ROM_PATH).map
ROM_SYM_FILE=$(ROM_PATH).sym

################################################################################
# Rules
#
# This section contains all the build rules.
# There is no need to edit this section.
################################################################################

.INTERMEDIATE: $(ROM_OBJECTS)
.PHONY: default fix launch link rom romd romr

default: romr

launch:
	@bgb --watch $(ROM_FILE) $(ROM_LAUNCH_OPTIONS) &

link: $(ROM_OBJECTS)
	@rgblink $(ROM_LINK_OPTIONS) --map $(ROM_MAP_FILE) --sym $(ROM_SYM_FILE) -o $(ROM_FILE) $(ROM_OBJECTS)

fix: link
	@rgbfix $(ROM_FIX_OPTIONS) --title $(ROM_GAME_NAME) $(ROM_FILE)

rom: fix

romr: ROM_COMPILE_OPTIONS += -DBUILD_RELEASE
romr: rom

romd: ROM_COMPILE_OPTIONS += -DBUILD_DEBUG
romd: rom

%.o: %.$(ROM_SRC_EXTENSION)
	@rgbasm $(ROM_COMPILE_OPTIONS) -i $(ROM_SRC_DIR) -o $@ $<


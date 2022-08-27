################################################################################
# Parameters
################################################################################

ROM_GAME_NAME=printer
ROM_SRC_DIR=src/
ROM_DIR=rom/
ROM_EXTENSION=gbc
ROM_COMPILE_OPTIONS=-Werror -Weverything
ROM_COMPILE_DEFINES=
ROM_LINK_OPTIONS=--tiny
ROM_FIX_OPTIONS=--title $(ROM_GAME_NAME) --color-only --pad-value 0 --validate
ROM_LAUNCH_OPTIONS=--listen 8765
ROM_FILES=\
	interrupt \
	main \
	memory \
	printer \
	protocol

################################################################################
# Inferences
################################################################################

ROM_OBJECTS=$(addprefix $(ROM_SRC_DIR),$(patsubst %,%.o,$(ROM_FILES)))

ROM_PATH=$(ROM_DIR)$(ROM_GAME_NAME)
ROM_FILE=$(ROM_PATH).$(ROM_EXTENSION)
ROM_MAP_FILE=$(ROM_PATH).map
ROM_SYM_FILE=$(ROM_PATH).sym

################################################################################
# Rules
################################################################################

.INTERMEDIATE: $(ROM_OBJECTS)
.PHONY: default fix launch link rom romd romr

default: romr

launch:
	@bgb --watch $(ROM_FILE) $(ROM_LAUNCH_OPTIONS) &

link: $(ROM_OBJECTS)
	@rgblink $(ROM_LINK_OPTIONS) --map $(ROM_MAP_FILE) --sym $(ROM_SYM_FILE) -o $(ROM_FILE) $(ROM_OBJECTS)

fix: link
	@rgbfix $(ROM_FIX_OPTIONS) $(ROM_FILE)

rom: fix

romr: ROM_COMPILE_DEFINES += -DBUILD_RELEASE
romr: rom

romd: ROM_COMPILE_DEFINES += -DBUILD_DEBUG
romd: rom

%.o: %.rgbasm
	@rgbasm $(ROM_COMPILE_OPTIONS) $(ROM_COMPILE_DEFINES) -i $(ROM_SRC_DIR) -o $@ $<


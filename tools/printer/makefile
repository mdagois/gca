.PHONY: default
default: rom

################################################################################
# ROM
################################################################################

# Parameters

ROM_GAME_NAME=printer
ROM_SRC_DIR=src/
ROM_DIR=rom/
ROM_EXTENSION=gb
ROM_COMPILE_OPTIONS=-Werror -Weverything -DIS_DEBUG
ROM_LINK_OPTIONS=--dmg --tiny
ROM_FIX_OPTIONS=--title $(ROM_GAME_NAME) --pad-value 0 --validate
ROM_LAUNCH_OPTIONS=--listen 8765
ROM_FILES=\
	interrupt \
	main \
	memory \
	printer \
	protocol

# Inferences

ROM_OBJECTS=$(addprefix $(ROM_SRC_DIR),$(patsubst %,%.o,$(ROM_FILES)))

ROM_PATH=$(ROM_DIR)$(ROM_GAME_NAME)
ROM_FILE=$(ROM_PATH).$(ROM_EXTENSION)
ROM_MAP_FILE=$(ROM_PATH).map
ROM_SYM_FILE=$(ROM_PATH).sym

# Rules

.INTERMEDIATE: $(ROM_OBJECTS)
.PHONY: fix launch link rom

launch:
	@bgb --watch $(ROM_FILE) $(ROM_LAUNCH_OPTIONS) &

link: $(ROM_OBJECTS)
	@rgblink $(ROM_LINK_OPTIONS) --map $(ROM_MAP_FILE) --sym $(ROM_SYM_FILE) -o $(ROM_FILE) $(ROM_OBJECTS)

fix: link
	@rgbfix $(ROM_FIX_OPTIONS) $(ROM_FILE)

rom: fix

%.o: %.rgbasm
	@rgbasm $(ROM_COMPILE_OPTIONS) -i $(ROM_SRC_DIR) -o $@ $<


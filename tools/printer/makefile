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
ROM_LINK_OPTIONS=--dmg --tiny
ROM_FIX_OPTIONS=
ROM_LAUNCH_OPTIONS=--listen 8765
ROM_FILES=\
	data \
	interrupt \
	main \
	memory \
	printer

# Inferences

ROM_OBJECTS=$(addprefix $(ROM_SRC_DIR),$(patsubst %,%.o,$(ROM_FILES)))

ROM_PATH=$(ROM_DIR)$(ROM_GAME_NAME)
ROM_FILE=$(ROM_PATH).$(ROM_EXTENSION)
ROM_MAP_FILE=$(ROM_PATH).map
ROM_SYM_FILE=$(ROM_PATH).sym

# Rules

.INTERMEDIATE: $(ROM_OBJECTS)
.PHONY: launch rom link fix

launch: rom
	@bgb --watch $(ROM_FILE) $(ROM_LAUNCH_OPTIONS) &

link: $(ROM_OBJECTS)
	@rgblink $(ROM_LINK_OPTIONS) --map $(ROM_MAP_FILE) --sym $(ROM_SYM_FILE) -o $(ROM_FILE) $(ROM_OBJECTS)

fix: link
	@rgbfix --title $(ROM_GAME_NAME) --pad-value 0 --validate $(ROM_FIX_OPTIONS) $(ROM_FILE)

rom: fix

%.o: %.rgbasm
	@rgbasm -Werror -Weverything -i $(ROM_SRC_DIR) -o $@ $<


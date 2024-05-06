projects = printer
configurations = debug release
default_target = all

build_directory = build

debug_description = build with debug features and extra checks enabled
release_description = deliverable build

compile_options = -Werror -Weverything
debug_compile_options = -DBUILD_DEBUG
release_compile_options = -DBUILD_RELEASE

link_options = --tiny --pad 0

fix_options = --color-only --pad-value 0 --validate

launch_options = --watch --listen 8765

########################################

printer_description = a printer simulator
printer_sources = $(wildcard src/*.rgbasm)

########################################

include gbbs.mk


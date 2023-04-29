#################################################################################
#
# Game Boy Build System (GBBS)
#
# Requirements:
# - RGBDS toolchain (`rgbasm`, `rgblink` and `rgbfix`)
# - make 4.0 or later
# - Shell commands (`mkdir`, `rm` and `touch`)
#
#################################################################################

# secondary expansion is heavily used in rule generation
.SECONDEXPANSION:

########################################
# Helper functions
########################################

# define true and false
true := TRUE
false :=

# turn the relative path $1 into an absolute path
expand_path = $(abspath $(strip $1))

# reverse a boolean
not = $(if $1,$(false),$(true))

# generate a fatal error with message $2 if the condition $1 is false
# asserts can be disabled by adding 'disable_asserts=1' to the make command (e.g. make all disable_asserts=1)
report_assert = $(if $(disable_asserts),,$(error $1))
assert = $(if $2,$(if $1,,$(call report_assert,Assertion failed: $2)),$(warning The assertion is ignored as it does not have any error message))

# string comparison functions
string_equal = $(if $(subst x$1,,x$2)$(subst x$2,,x$1),$(false),$(true))
string_not_equal = $(call not,$(call string_equal,$1,$2))

########################################
# Default target
########################################

default_target ?= help
.DEFAULT_GOAL := $(default_target)
$(call assert,$(call not,$(filter clean%,$(default_target))),Clean operations are not supported as default targets)

########################################
# Target checks
########################################

# cut parallel execution for clean rules to avoid any issue
ifneq ($(filter clean%,$(MAKECMDGOALS)),)
.NOTPARALLEL:
ifneq ($(filter-out clean%,$(MAKECMDGOALS)),)
keep_rules_makefile := ${true}
endif
endif

########################################
# Solution variables
########################################

projects := $(strip $(projects))
configurations := $(strip $(configurations))
build_directory := $(call expand_path,$(build_directory))
root_directory := $(call expand_path,.)

$(call assert,$(projects),The 'projects' list is empty)
$(call assert,$(configurations),The 'configurations' list is empty)
$(call assert,$(build_directory),The 'build_directory' is not valid)

########################################
# Constants
########################################

# extensions
object_extension := .o
dependency_extension := .d
signature_extension := .s
force_extension := .f

# file commands
shell_command_directory_with_slash := $(if $(shell_command_directory),$(shell_command_directory)/,)
mkdir_command=$(shell_command_directory_with_slash)mkdir -p
rmdir_command=$(shell_command_directory_with_slash)rm -rf
touch_command=$(shell_command_directory_with_slash)touch

# toolchain commands
rgbds_directory_with_slash := $(if $(rgbds_directory),$(rgbds_directory)/,)
rgbasm_command=$(rgbds_directory_with_slash)rgbasm
rgblink_command=$(rgbds_directory_with_slash)rgblink
rgbfix_command=$(rgbds_directory_with_slash)rgbfix
emulator_command ?= bgb

########################################
# General rules
########################################

# all temporary files are held into a temporary directory under the build directory
temporary_directory := .temp
$(call assert,$(call not,$(filter $(temporary_directory),$(configurations))),'$(temporary_directory)' is a reserved word and cannot be used as a configuration name)

# directory creation rule
%/:
	@$(mkdir_command) $(dir $@)

# build echo rule
.PHONY: build_echo_%
build_echo_%:
	@echo [BUILD] $*

################################################################################
# Build commands
################################################################################

# compilation
# $1 = project, $2 = configuration, $3 = output object file, $4 = input source file
compile_command = $$(rgbasm_command) $$($1_$2_compile_options_list) -I$$(dir $4) -M $(3:=$(dependency_extension)) -MG -MP --output $3 $4

# linkage
# $1 = project, $2 = configuration, $3 = output binary file, $4 = input object files list
link_command = $$(rgblink_command) $$($1_$2_link_options_list) --map $$($1_$2_map_file) --sym $$($1_$2_sym_file) --output $3 $4

# fix
# $1 = project, $2 = configuration, $3 = rom file
fix_command = $$(rgbfix_command) $$($1_$2_fix_options_list) --title $1 $3

########################################
# Generation templates
########################################

# header
define rule_file_header_template
################################################################################
# GENERATED RULES
################################################################################
.SECONDEXPANSION:


endef

# solution
define solution_specific_template
########################################
# Solution
########################################

.PHONY: all clean
all:
clean:
	$$(if $$(call not,$$(keep_rules_makefile)),$$(rmdir_command) $$(build_directory))


endef

# project
# $1 = project
define project_specific_template
########################################
# Rules for project '$1'
########################################

$1_description ?= no description

$1_build_directory = $(build_directory)/$(temporary_directory)/$1

all: $1
clean: clean_$1

.PHONY: $1 clean_$1
clean_$1:
	$$(rmdir_command) $$($1_build_directory)


endef

# configuration
# $1 = configuration
define configuration_specific_template
########################################
# Rules for configuration '$1'
########################################

$1_description ?= no description
$1_binary_directory = $(build_directory)/$1

.PHONY: $1 clean_$1
clean_$1: $(addsuffix _$1,$(addprefix clean_,$(projects)))
clean_$1:
	$$(rmdir_command) $$($1_binary_directory)


endef

# target variables
# $1 = project, $2 = configuration
define target_variable_definitions_template
########################################
# Variables for '$1_$2'
########################################

$1_$2_sources_list = $$($1_sources) $$($1_$2_sources)

$1_$2_compile_options_list = -I$(root_directory) $(compile_options) $$($1_compile_options) $$($2_compile_options) $$($1_$2_compile_options)
$1_$2_link_options_list = $(link_options) $$($1_link_options) $$($2_link_options) $$($1_$2_link_options)
$1_$2_fix_options_list = $(fix_options) $$($1_fix_options) $$($2_fix_options) $$($1_$2_fix_options)

$1_$2_launch_options_list = $$(strip $(launch_options) $$($1_launch_options) $$($2_launch_options) $$($1_$2_launch_options))
$1_$2_launch_options_list2 = $$(strip $(launch_options2) $$($1_launch_options2) $$($2_launch_options2) $$($1_$2_launch_options2))

$1_$2_rom_extension := $$(if $$(filter --color-only --color-compatible -C -c,$$($1_$2_fix_options_list)),.gbc,.gb)

$1_$2_build_directory := $$($1_build_directory)/$2
$1_$2_binary_directory := $$($2_binary_directory)/$1
$1_$2_binary_basename := $$($1_$2_binary_directory)/$1
$1_$2_binary := $$($1_$2_binary_basename)$$($1_$2_rom_extension)
$1_$2_binary_signature := $$($1_$2_build_directory)/$1$(signature_extension)
$1_$2_binary_force := $$($1_$2_build_directory)/$1$(force_extension)
$1_$2_map_file := $$($1_$2_binary_basename).map
$1_$2_sym_file := $$($1_$2_binary_basename).sym

$1_$2_objects := $$(addprefix $$($1_$2_build_directory)/,$$($1_$2_sources_list:=$(object_extension)))
$1_$2_dependencies := $$($1_$2_objects:=$(dependency_extension))
$1_$2_signatures := $$($1_$2_objects:=$(signature_extension)) $$($1_$2_binary_signature)

$$(call assert,$$(strip $$($1_$2_sources_list)),No source files for target '$1_$2' (add source files to '$1_sources' or '$1_$2_sources'))


endef

# signature
# $1 = project, $2 = configuration, $3 = target, $4 = signature, $5 = force, $6 = expanded command, $7 = non-expanded command
define signature_template
$$(if $$(wildcard $$(dir $4)),\
$$(file >$4,$5:)\
$$(file >>$4,$3: $5)\
$$(file >>$4,$$$$(if $$$$(call string_not_equal,$6,$7),$$$$(shell $(touch_command) $5)))\
$$(file >$5)\
)
endef

# target rules
# $1 = project, $2 = configuration
define target_rule_definitions_template
########################################
# Rules for '$1_$2'
########################################

.PHONY: $1_$2 clean_$1_$2

clean_$1: clean_$1_$2

$1: $1_$2
$2: $1_$2
$1_$2: | build_echo_$1_$2
$1_$2: $$($1_$2_binary)

$$($1_$2_binary): | $$($1_$2_build_directory)/ $$($1_$2_binary_directory)/
$$($1_$2_binary): $$($1_$2_objects)
	$(call signature_template,$1,$2,$$@,$$($1_$2_binary_signature),$$($1_$2_binary_force),$(call link_command,$1,$2,$$@,$$($1_$2_objects)) && $(call fix_command,$1,$2,$$@),$$(call link_command,$1,$2,$$@,$$($1_$2_objects)) && $$(call fix_command,$1,$2,$$@))
	$(call link_command,$1,$2,$$@,$$($1_$2_objects)) && $(call fix_command,$1,$2,$$@)

$$($1_$2_build_directory)/%$(object_extension): $$(root_directory)/% | $$$$(@D)/
	$(call signature_template,$1,$2,$$@,$$($1_$2_build_directory)/$$*$(object_extension)$(signature_extension),$$($1_$2_build_directory)/$$*$(object_extension)$(force_extension),$(call compile_command,$1,$2,$$@,$$<),$$(call compile_command,$1,$2,$$@,$$<))
	$(call compile_command,$1,$2,$$@,$$<)

launch_$1_$2:
	$(emulator_command) $($1_$2_launch_options_list) $($1_$2_binary) &
	$(if $($1_$2_launch_options_list2),$(emulator_command) $($1_$2_launch_options_list2) $($1_$2_binary) &,)

clean_$1_$2:
	$$(rmdir_command) $$($1_$2_build_directory)
	$$(rmdir_command) $$($1_$2_binary_directory)

-include $$($1_$2_dependencies)
-include $$($1_$2_signatures)


endef

########################################
# Generation
########################################

# file containing all the generated rules
rules_makefile := $(build_directory)/$(temporary_directory)/rules.mk

# rule file generation
$(rules_makefile): $(MAKEFILE_LIST) | $$(@D)/
	$(file >$@,$(call rule_file_header_template))
	$(file >>$@,$(call solution_specific_template))
	$(foreach project,$(projects),$(file >>$@,$(call project_specific_template,$(project))))
	$(foreach configuration,$(configurations),$(file >>$@,$(call configuration_specific_template,$(configuration))))
	$(foreach project,$(projects),$(foreach configuration,$(configurations),$(file >>$@,$(call target_variable_definitions_template,$(project),$(configuration)))))
	$(foreach project,$(projects),$(foreach configuration,$(configurations),$(file >>$@,$(call target_rule_definitions_template,$(project),$(configuration)))))

-include $(rules_makefile)

################################################################################
# Help
################################################################################

# rule to display help
.PHONY: help
help:
	$(info ========== Help ==========)
	$(info Projects)
	$(foreach project,$(projects),$(info - $(project): $($(project)_description)))
	$(info Configurations)
	$(foreach configuration,$(configurations),$(info - $(configuration): $($(configuration)_description)))
	$(info Build targets)
	$(info - all: $(projects))
	$(foreach project,$(projects),$(info - $(project): $(addprefix $(project)_,$(configurations))))
	$(foreach configuration,$(configurations),$(info - $(configuration): $(addsuffix _$(configuration),$(projects))))
	$(foreach target,$(foreach project,$(projects),$(addprefix $(project)_,$(configurations))),$(info - $(target)))
	$(info Clean targets)
	$(info - clean)
	$(foreach project,$(projects),$(info - clean_$(project)))
	$(foreach configuration,$(configurations),$(info - clean_$(configuration)))
	$(foreach target,$(foreach project,$(projects),$(addprefix $(project)_,$(configurations))),$(info - clean_$(target)))
	$(info Launch targets)
	$(foreach target,$(foreach project,$(projects),$(addprefix $(project)_,$(configurations))),$(info - launch_$(target)))
	$(info ==========================)

################################################################################
# Debug
################################################################################

.PHONY: printvars print-%

# print all variables (for debugging)
printvars:
	$(foreach variable,$(sort $(.VARIABLES)),\
		$(if $(filter-out environment% default automatic,$(origin $(variable))),\
			$(if $(filter-out %_template assert,$(variable)),$(info $(variable) = $($(variable))))\
		)\
	)

# print information about a single variable (for debugging)
print-%:
	$(info $* = '$($*)')
	$(info origin = '$(origin $*)')
	$(info value = '$(value $*)')


#################################################################################
#
# Game Boy Build System (GBBS)
#
# Requirements:
# - RGBDS toolchain (`rgbasm`, `rgblink` and `rgbfix`)
# - make 4.0 or later
# - Git (for `mkdir` and `rm` commands)
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
assert = $(if $2,$(if $1,,$(error Assertion failed: $2)),$(warning The assertion is ignored as it does not have any error message))

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
source_directory := $(call expand_path,$(source_directory))
build_directory := $(call expand_path,$(build_directory))

$(call assert,$(projects),The 'projects' list is empty)
$(call assert,$(configurations),The 'configurations' list is empty)
$(call assert,$(source_directory),The 'source_directory' is not valid)
$(call assert,$(wildcard $(source_directory)),The 'source_directory' does not exist)
$(call assert,$(build_directory),The 'build_directory' is not valid)
$(call assert,$(call string_not_equal,$(source_directory),$(build_directory)),'source_directory' and 'build_directory' must be different)
$(call assert,$(call string_equal,$(subst $(build_directory),,$(source_directory)),$(source_directory)),The 'build_directory' must not be a parent of the source directory (it must be completely dedicated to holding build files))

########################################
# Constants
########################################

# extension
object_extension := .o

# file commands
mkdir_command=$(git_directory)mkdir -p
rmdir_command=$(git_directory)rm -rf

# toolchain commands
rgbasm_command=$(rgbds_directory)rgbasm
rgblink_command=$(rgbds_directory)rgblink
rgbfix_command=$(rgbds_directory)rgbfix
emulator_command=$(rgbds_directory)bgb

########################################
# Directory rules
########################################

# all temporary files are held into a temporary directory under the build directory
temporary_directory := .temp
$(call assert,$(call not,$(filter $(temporary_directory),$(configurations))),'$(temporary_directory)' is a reserved word and cannot be used as a configuration name)

# directory creation rule
%/:
	@$(mkdir_command) $(dir $@)

################################################################################
# Build commands
################################################################################

# compilation
# $1 = project, $2 = configuration, $3 = output object file, $4 = input source file
compile_command = $$(rgbasm_command) $$($1_$2_compile_options_list) --output $3 $4

# linkage
# $1 = project, $2 = configuration, $3 = output binary file, $4 = input object files list
link_command = $$(rgblink_command) $$($1_$2_link_options_list) --map $$($1_$2_map_file) --sym $$($1_$2_sym_file) --output $3$$($1_$2_rom_extension) $4

# fix
# $1 = project, $2 = configuration, $3 = rom file
fix_command = $$(rgbfix_command) $$($1_$2_fix_options_list) --title $1 $3$$($1_$2_rom_extension)

# launch
# $1 = project, $2 = configuration, $3 = option list number, $4 = force flag
launch_command = $$(if $$($1_$2_launch_options_list$3) $4,$$(emulator_command) $$($1_$2_launch_options_list$3) $$($1_$2_binary)$$($1_$2_rom_extension) &,)

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
	@$$(if $$(call not,$$(keep_rules_makefile)),@$$(rmdir_command) $$(build_directory))


endef

# project
# $1 = project
define project_specific_template
########################################
# Rules for project '$1'
########################################

$1_description ?= no description

$1_source_directory = $(source_directory)/$$($1_source_sub_directory)
$1_build_directory = $(build_directory)/$(temporary_directory)/$1

all: $1
clean: clean_$1
$1_targets: ;

.PHONY: $1 clean_$1 $1_targets
clean_$1:
	@$$(rmdir_command) $$($1_build_directory)

$$(call assert,$$(wildcard $$($1_source_directory)),The 'source_sub_directory' for project '$1' does not exist (current value is '$$($1_source_sub_directory)'))


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
	@$$(rmdir_command) $$($1_binary_directory)


endef

# target variables
# $1 = project, $2 = configuration
define target_variable_definitions_template
########################################
# Variables for '$1_$2'
########################################

$1_$2_sources_list = $$($1_sources) $$($1_$2_sources)

$1_$2_compile_options_list = -I$$($1_source_directory) -I$(source_directory) $(compile_options) $$($1_compile_options) $$($2_compile_options) $$($1_$2_compile_options)
$1_$2_link_options_list = $(link_options) $$($1_link_options) $$($2_link_options) $$($1_$2_link_options)
$1_$2_fix_options_list = $(fix_options) $$($1_fix_options) $$($2_fix_options) $$($1_$2_fix_options)

$1_$2_launch_options_list = $$(strip $$($1_launch_options) $$($2_launch_options) $$($1_$2_launch_options))
$1_$2_launch_options_list2 = $$(strip $$($1_launch_options2) $$($2_launch_options2) $$($1_$2_launch_options2))

$1_$2_build_directory := $$($1_build_directory)/$2
$1_$2_binary_directory := $$($2_binary_directory)/$1
$1_$2_binary := $$($1_$2_binary_directory)/$1
$1_$2_map_file := $$($1_$2_binary).map
$1_$2_sym_file := $$($1_$2_binary).sym

$1_$2_rom_extension := $$(if $$(filter --color-only --color-compatible -C -c,$$($1_$2_fix_options_list)),.gbc,.gb)

$1_$2_objects := $$(addprefix $$($1_$2_build_directory)/,$$($1_$2_sources_list:=$(object_extension)))
.INTERMEDIATE: $$($1_$2_objects)

$$(call assert,$$(strip $$($1_$2_sources_list)),No source files for target '$1_$2' (add source files to '$1_sources' or '$1_$2_sources'))


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
$1_$2: $$($1_$2_binary)

$$($1_$2_binary): | $$($1_$2_build_directory)/ $$($1_$2_binary_directory)/
$$($1_$2_binary): $$($1_$2_objects) | $1_targets 
	$(call link_command,$1,$2,$$@,$$^) && $(call fix_command,$1,$2,$$@)

$$($1_$2_build_directory)/%$(object_extension): $$($1_source_directory)/% | $$$$(@D)/
	$(call compile_command,$1,$2,$$@,$$<)

launch_$1_$2:
	@$(call launch_command,$1,$2,,$(true))
	@$(call launch_command,$1,$2,2,$(false))

clean_$1_$2:
	@$$(rmdir_command) $$($1_$2_build_directory)
	@$$(rmdir_command) $$($1_$2_binary_directory)


endef

########################################
# Generation
########################################

# file containing all the generated rules
rules_makefile := $(build_directory)/$(temporary_directory)/rules.mk

# rule file generation
$(rules_makefile): $(MAKEFILE_LIST) | $$(@D)/
	@$(file >$@,$(call rule_file_header_template))
	@$(file >>$@,$(call solution_specific_template))
	@$(foreach project,$(projects),$(file >>$@,$(call project_specific_template,$(project))))
	@$(foreach configuration,$(configurations),$(file >>$@,$(call configuration_specific_template,$(configuration))))
	@$(foreach project,$(projects),$(foreach configuration,$(configurations),$(file >>$@,$(call target_variable_definitions_template,$(project),$(configuration)))))
	@$(foreach project,$(projects),$(foreach configuration,$(configurations),$(file >>$@,$(call target_rule_definitions_template,$(project),$(configuration)))))

-include $(rules_makefile)

################################################################################
# Help
################################################################################

# rule to display help
.PHONY: help
help:
	@$(info ========== Help ==========)
	@$(info Projects)
	@$(foreach project,$(projects),$(info - $(project): $($(project)_description)))
	@$(info Configurations)
	@$(foreach configuration,$(configurations),$(info - $(configuration): $($(configuration)_description)))
	@$(info Build targets)
	@$(info - all: $(projects))
	@$(foreach project,$(projects),$(info - $(project): $(addprefix $(project)_,$(configurations))))
	@$(foreach configuration,$(configurations),$(info - $(configuration): $(addsuffix _$(configuration),$(projects))))
	@$(foreach target,$(foreach project,$(projects),$(addprefix $(project)_,$(configurations))),$(info - $(target)))
	@$(info Clean targets)
	@$(info - clean)
	@$(foreach project,$(projects),$(info - clean_$(project)))
	@$(foreach configuration,$(configurations),$(info - clean_$(configuration)))
	@$(foreach target,$(foreach project,$(projects),$(addprefix $(project)_,$(configurations))),$(info - clean_$(target)))
	@$(info Launch targets)
	@$(foreach target,$(foreach project,$(projects),$(addprefix $(project)_,$(configurations))),$(info - launch_$(target)))
	@$(info ==========================)

################################################################################
# Debug
################################################################################

.PHONY: printvars print-%

# print all variables (for debugging)
printvars:
	@$(foreach variable,$(sort $(.VARIABLES)),\
		$(if $(filter-out environment% default automatic,$(origin $(variable))),\
			$(if $(filter-out %_template assert,$(variable)),$(info $(variable) = $($(variable))))\
		)\
	)

# print information about a single variable (for debugging)
print-%:
	@$(info $* = '$($*)')
	@$(info origin = '$(origin $*)')
	@$(info value = '$(value $*)')


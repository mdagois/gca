projects = \
	base \
	bg \
	catch \
	cgb_bg \
	cgb_compatibility \
	cgb_cpu_speed \
	cgb_dma_gp \
	cgb_dma_hb \
	cgb_palettes \
	cgb_priority \
	cgb_sprites \
	ch1_continuous \
	ch1_duty \
	ch1_envelope \
	ch1_frequency \
	ch1_full_test \
	ch1_length \
	ch1_simple \
	ch1_sweep \
	ch1_volume \
	ch2_full_test \
	ch3_full_test \
	ch3_simple \
	ch3_waveforms \
	ch4_example \
	ch4_full_test \
	counter_rng \
	dma \
	dma_double_sprite \
	dma_none \
	first_program \
	init \
	input_bg \
	input_interrupt \
	input_window \
	interrupt_trigger \
	jump_table \
	jump_table_equivalent \
	lfsr \
	mbc_eram \
	mbc_rom \
	palette_fade \
	parallax \
	parallax_optimized \
	printer \
	scrolling_anim_broken \
	scrolling_anim_fixed \
	serial_multibytes \
	serial_multibytes_timer \
	serial_parent_child \
	serial_switch_roles \
	serial_transfer \
	sgb_border_base \
	sgb_border_base_fixed \
	sgb_border_fullscreen \
	sgb_border_overlap \
	sgb_color_base \
	sgb_color_index \
	sgb_color_transfer_combined \
	sgb_color_transfer_index \
	sgb_color_transfer_pal \
	sgb_detection \
	sgb_init \
	sgb_multi \
	sgb_settings \
	sgb_sound_test \
	sound_test \
	sprites \
	sprites_fixed \
	timer_sec \
	window

configurations = release
default_target = all

build_directory = ../build-samples

release_description = deliverable build

compile_options = -Werror -Weverything
fix_options = --validate

launch_options = --watch

PORT = 8765

########################################

base_sources = $(wildcard base/*.rgbasm)
base_link_options = --dmg --tiny
base_fix_options = --pad-value 0

bg_sources = $(wildcard bg/*.rgbasm)
bg_link_options = --dmg --tiny
bg_fix_options = --pad-value 0

catch_sources = $(wildcard catch/*.rgbasm)
catch_link_options = --pad 0xC7 --dmg --tiny
catch_fix_options = --pad-value 0xC7

cgb_bg_sources = $(wildcard cgb_bg/*.rgbasm)
cgb_bg_link_options = --tiny
cgb_bg_fix_options = --color-only --pad-value 0

cgb_compatibility_sources = $(wildcard cgb_compatibility/*.rgbasm)
cgb_compatibility_link_options = --tiny
cgb_compatibility_fix_options = --color-compatible --pad-value 0

cgb_cpu_speed_sources = $(wildcard cgb_cpu_speed/*.rgbasm)
cgb_cpu_speed_link_options = --tiny
cgb_cpu_speed_fix_options = --color-only --pad-value 0

cgb_dma_gp_sources = $(wildcard cgb_dma_gp/*.rgbasm)
cgb_dma_gp_link_options = --tiny
cgb_dma_gp_fix_options = --color-only --pad-value 0

cgb_dma_hb_sources = $(wildcard cgb_dma_hb/*.rgbasm)
cgb_dma_hb_link_options = --tiny
cgb_dma_hb_fix_options = --color-only --pad-value 0

cgb_palettes_sources = $(wildcard cgb_palettes/*.rgbasm)
cgb_palettes_link_options = --tiny
cgb_palettes_fix_options = --color-only --pad-value 0

cgb_priority_sources = $(wildcard cgb_priority/*.rgbasm)
cgb_priority_link_options = --tiny
cgb_priority_fix_options = --color-only --pad-value 0

cgb_sprites_sources = $(wildcard cgb_sprites/*.rgbasm)
cgb_sprites_link_options = --tiny
cgb_sprites_fix_options = --color-only --pad-value 0

ch1_continuous_sources = $(wildcard ch1_continuous/*.rgbasm)
ch1_continuous_link_options = --dmg --tiny
ch1_continuous_fix_options = --pad-value 0

ch1_duty_sources = $(wildcard ch1_duty/*.rgbasm)
ch1_duty_link_options = --dmg --tiny
ch1_duty_fix_options = --pad-value 0

ch1_envelope_sources = $(wildcard ch1_envelope/*.rgbasm)
ch1_envelope_link_options = --dmg --tiny
ch1_envelope_fix_options = --pad-value 0

ch1_frequency_sources = $(wildcard ch1_frequency/*.rgbasm)
ch1_frequency_link_options = --dmg --tiny
ch1_frequency_fix_options = --pad-value 0

ch1_full_test_sources = $(wildcard ch1_full_test/*.rgbasm)
ch1_full_test_link_options = --dmg --tiny
ch1_full_test_fix_options = --pad-value 0

ch1_length_sources = $(wildcard ch1_length/*.rgbasm)
ch1_length_link_options = --dmg --tiny
ch1_length_fix_options = --pad-value 0

ch1_simple_sources = $(wildcard ch1_simple/*.rgbasm)
ch1_simple_link_options = --dmg --tiny
ch1_simple_fix_options = --pad-value 0

ch1_sweep_sources = $(wildcard ch1_sweep/*.rgbasm)
ch1_sweep_link_options = --dmg --tiny
ch1_sweep_fix_options = --pad-value 0

ch1_volume_sources = $(wildcard ch1_volume/*.rgbasm)
ch1_volume_link_options = --dmg --tiny
ch1_volume_fix_options = --pad-value 0

ch2_full_test_sources = $(wildcard ch2_full_test/*.rgbasm)
ch2_full_test_link_options = --dmg --tiny
ch2_full_test_fix_options = --pad-value 0

ch3_full_test_sources = $(wildcard ch3_full_test/*.rgbasm)
ch3_full_test_link_options = --dmg --tiny
ch3_full_test_fix_options = --pad-value 0

ch3_simple_sources = $(wildcard ch3_simple/*.rgbasm)
ch3_simple_link_options = --dmg --tiny
ch3_simple_fix_options = --pad-value 0

ch3_waveforms_sources = $(wildcard ch3_waveforms/*.rgbasm)
ch3_waveforms_link_options = --dmg --tiny
ch3_waveforms_fix_options = --pad-value 0

ch4_example_sources = $(wildcard ch4_example/*.rgbasm)
ch4_example_link_options = --dmg --tiny
ch4_example_fix_options = --pad-value 0

ch4_full_test_sources = $(wildcard ch4_full_test/*.rgbasm)
ch4_full_test_link_options = --dmg --tiny
ch4_full_test_fix_options = --pad-value 0

counter_rng_sources = $(wildcard counter_rng/*.rgbasm)
counter_rng_link_options = --dmg --tiny
counter_rng_fix_options = --pad-value 0

dma_sources = $(wildcard dma/*.rgbasm)
dma_link_options = --dmg --tiny
dma_fix_options = --pad-value 0

dma_double_sprite_sources = $(wildcard dma_double_sprite/*.rgbasm)
dma_double_sprite_link_options = --dmg --tiny
dma_double_sprite_fix_options = --pad-value 0

dma_none_sources = $(wildcard dma_none/*.rgbasm)
dma_none_link_options = --dmg --tiny
dma_none_fix_options = --pad-value 0

first_program_sources = $(wildcard first_program/*.rgbasm)
first_program_link_options = --dmg --tiny
first_program_fix_options = --pad-value 0

init_sources = $(wildcard init/*.rgbasm)
init_link_options = --dmg --tiny
init_fix_options = --pad-value 0

input_bg_sources = $(wildcard input_bg/*.rgbasm)
input_bg_link_options = --dmg --tiny
input_bg_fix_options = --pad-value 0

input_interrupt_sources = $(wildcard input_interrupt/*.rgbasm)
input_interrupt_link_options = --dmg --tiny
input_interrupt_fix_options = --pad-value 0

input_window_sources = $(wildcard input_window/*.rgbasm)
input_window_link_options = --dmg --tiny
input_window_fix_options = --pad-value 0

interrupt_trigger_sources = $(wildcard interrupt_trigger/*.rgbasm)
interrupt_trigger_link_options = --dmg --tiny
interrupt_trigger_fix_options = --pad-value 0

jump_table_sources = $(wildcard jump_table/*.rgbasm)
jump_table_link_options = --dmg --tiny
jump_table_fix_options = --pad-value 0

jump_table_equivalent_sources = $(wildcard jump_table_equivalent/*.rgbasm)
jump_table_equivalent_link_options = --dmg --tiny
jump_table_equivalent_fix_options = --pad-value 0

lfsr_sources = $(wildcard lfsr/*.rgbasm)
lfsr_link_options = --dmg --tiny
lfsr_fix_options = --pad-value 0

mbc_eram_sources = $(wildcard mbc_eram/*.rgbasm)
mbc_eram_link_options = --dmg
mbc_eram_fix_options = --mbc-type 0x1B --ram-size 0x03 --pad-value 0

mbc_rom_sources = $(wildcard mbc_rom/*.rgbasm)
mbc_rom_link_options = --dmg
mbc_rom_fix_options = --mbc-type 0x19 --pad-value 0

palette_fade_sources = $(wildcard palette_fade/*.rgbasm)
palette_fade_link_options = --dmg --tiny
palette_fade_fix_options = --pad-value 0

parallax_sources = $(wildcard parallax/*.rgbasm)
parallax_link_options = --dmg --tiny
parallax_fix_options = --pad-value 0

parallax_optimized_sources = $(wildcard parallax_optimized/*.rgbasm)
parallax_optimized_link_options = --dmg --tiny
parallax_optimized_fix_options = --pad-value 0

printer_sources = $(wildcard printer/*.rgbasm)
printer_link_options = --dmg --tiny
printer_fix_options = --pad-value 0
printer_launch_options = --connect 127.0.0.1:$(PORT)

scrolling_anim_broken_sources = $(wildcard scrolling_anim_broken/*.rgbasm)
scrolling_anim_broken_link_options = --dmg --tiny
scrolling_anim_broken_fix_options = --pad-value 0

scrolling_anim_fixed_sources = $(wildcard scrolling_anim_fixed/*.rgbasm)
scrolling_anim_fixed_link_options = --dmg --tiny
scrolling_anim_fixed_fix_options = --pad-value 0

serial_multibytes_sources = $(wildcard serial_multibytes/*.rgbasm)
serial_multibytes_link_options = --dmg --tiny
serial_multibytes_fix_options = --pad-value 0
serial_multibytes_launch_options = --listen $(PORT)
serial_multibytes_launch_options2 = --connect 127.0.0.1:$(PORT)

serial_multibytes_timer_sources = $(wildcard serial_multibytes_timer/*.rgbasm)
serial_multibytes_timer_link_options = --dmg --tiny
serial_multibytes_timer_fix_options = --pad-value 0
serial_multibytes_timer_launch_options = --listen $(PORT)
serial_multibytes_timer_launch_options2 = --connect 127.0.0.1:$(PORT)

serial_parent_child_sources = $(wildcard serial_parent_child/*.rgbasm)
serial_parent_child_link_options = --dmg --tiny
serial_parent_child_fix_options = --pad-value 0
serial_parent_child_launch_options = --listen $(PORT)
serial_parent_child_launch_options2 = --connect 127.0.0.1:$(PORT)

serial_switch_roles_sources = $(wildcard serial_switch_roles/*.rgbasm)
serial_switch_roles_link_options = --dmg --tiny
serial_switch_roles_fix_options = --pad-value 0
serial_switch_roles_launch_options = --listen $(PORT)
serial_switch_roles_launch_options2 = --connect 127.0.0.1:$(PORT)

serial_transfer_sources = $(wildcard serial_transfer/*.rgbasm)
serial_transfer_link_options = --dmg --tiny
serial_transfer_fix_options = --pad-value 0
serial_transfer_launch_options = --listen $(PORT)
serial_transfer_launch_options2 = --connect 127.0.0.1:$(PORT)

sgb_border_base_sources = $(wildcard sgb_border_base/*.rgbasm)
sgb_border_base_link_options = --tiny
sgb_border_base_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_border_base_fixed_sources = $(wildcard sgb_border_base_fixed/*.rgbasm)
sgb_border_base_fixed_link_options = --tiny
sgb_border_base_fixed_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_border_fullscreen_sources = $(wildcard sgb_border_fullscreen/*.rgbasm)
sgb_border_fullscreen_link_options = --tiny
sgb_border_fullscreen_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_border_overlap_sources = $(wildcard sgb_border_overlap/*.rgbasm)
sgb_border_overlap_link_options = --tiny
sgb_border_overlap_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_color_base_sources = $(wildcard sgb_color_base/*.rgbasm)
sgb_color_base_link_options = --tiny
sgb_color_base_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_color_index_sources = $(wildcard sgb_color_index/*.rgbasm)
sgb_color_index_link_options = --tiny
sgb_color_index_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_color_transfer_combined_sources = $(wildcard sgb_color_transfer_combined/*.rgbasm)
sgb_color_transfer_combined_link_options = --tiny
sgb_color_transfer_combined_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_color_transfer_index_sources = $(wildcard sgb_color_transfer_index/*.rgbasm)
sgb_color_transfer_index_link_options = --tiny
sgb_color_transfer_index_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_color_transfer_pal_sources = $(wildcard sgb_color_transfer_pal/*.rgbasm)
sgb_color_transfer_pal_link_options = --tiny
sgb_color_transfer_pal_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_detection_sources = $(wildcard sgb_detection/*.rgbasm)
sgb_detection_link_options = --tiny
sgb_detection_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_init_sources = $(wildcard sgb_init/*.rgbasm)
sgb_init_link_options = --tiny
sgb_init_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_multi_sources = $(wildcard sgb_multi/*.rgbasm)
sgb_multi_link_options = --tiny
sgb_multi_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_settings_sources = $(wildcard sgb_settings/*.rgbasm)
sgb_settings_link_options = --tiny
sgb_settings_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sgb_sound_test_sources = $(wildcard sgb_sound_test/*.rgbasm)
sgb_sound_test_link_options = --tiny
sgb_sound_test_fix_options = --sgb-compatible --old-licensee 0x33 --pad-value 0

sound_test_sources = $(wildcard sound_test/*.rgbasm)
sound_test_link_options = --dmg --tiny
sound_test_fix_options = --pad-value 0

sprites_sources = $(wildcard sprites/*.rgbasm)
sprites_link_options = --dmg --tiny
sprites_fix_options = --pad-value 0

sprites_fixed_sources = $(wildcard sprites_fixed/*.rgbasm)
sprites_fixed_link_options = --dmg --tiny
sprites_fixed_fix_options = --pad-value 0

timer_sec_sources = $(wildcard timer_sec/*.rgbasm)
timer_sec_link_options = --dmg --tiny
timer_sec_fix_options = --pad-value 0

window_sources = $(wildcard window/*.rgbasm)
window_link_options = --dmg --tiny
window_fix_options = --pad-value 0

########################################

include ../tools/gbbs.mk


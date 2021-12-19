echo off
cd "%~dp0"

set ASSETS_DIR=assets\
set TILESET_DIR=%ASSETS_DIR%tileset\
set TILEMAP_DIR=%ASSETS_DIR%tilemap\
set GFXCONV=bin\gfxconv.exe

%GFXCONV% %TILESET_DIR%tileset.png %TILEMAP_DIR%background.png %TILEMAP_DIR%ch1_continuous.png %TILEMAP_DIR%ch3_simple.png %TILEMAP_DIR%ch3_waveforms.png %TILEMAP_DIR%ch4_example.png %TILEMAP_DIR%counter_rng.png %TILEMAP_DIR%interrupt_trigger.png %TILEMAP_DIR%joypad.png %TILEMAP_DIR%serial_multibytes.png %TILEMAP_DIR%serial_parent_child.png %TILEMAP_DIR%serial_switch_roles.png %TILEMAP_DIR%serial_transfer.png %TILEMAP_DIR%sound_duty.png %TILEMAP_DIR%sound_envelope.png %TILEMAP_DIR%sound_frequency.png %TILEMAP_DIR%sound_length.png %TILEMAP_DIR%sound_octave.png %TILEMAP_DIR%sound_simple.png %TILEMAP_DIR%sound_sweep.png %TILEMAP_DIR%sound_volume.png %TILEMAP_DIR%timer_sec.png %TILEMAP_DIR%window.png
%GFXCONV% %TILESET_DIR%parallax.png %TILEMAP_DIR%parallax.png
%GFXCONV% %TILESET_DIR%sound_test.png %TILEMAP_DIR%ch1_test.png %TILEMAP_DIR%ch2_test.png %TILEMAP_DIR%ch3_test.png %TILEMAP_DIR%ch4_test.png


echo off
cd "%~dp0"

set ASSETS_DIR=assets\
set TILESET_DIR=%ASSETS_DIR%tileset\
set TILEMAP_DIR=%ASSETS_DIR%tilemap\
set GFXCONV=bin\gfxconv.exe

set GENERATED_DIR=generated\
set CHR_EXTENSION=chr
set CHR_DIR=%GENERATED_DIR%%CHR_EXTENSION%\
set TLM_EXTENSION=tlm
set TLM_DIR=%GENERATED_DIR%%TLM_EXTENSION%\

%GFXCONV% %TILESET_DIR%tileset.png %TILEMAP_DIR%background.png %TILEMAP_DIR%ch1_continuous.png %TILEMAP_DIR%ch3_simple.png %TILEMAP_DIR%ch3_waveforms.png %TILEMAP_DIR%ch4_example.png %TILEMAP_DIR%counter_rng.png %TILEMAP_DIR%frame.png %TILEMAP_DIR%input_interrupt.png %TILEMAP_DIR%interrupt_trigger.png %TILEMAP_DIR%serial_multibytes.png %TILEMAP_DIR%serial_parent_child.png %TILEMAP_DIR%serial_switch_roles.png %TILEMAP_DIR%serial_transfer.png %TILEMAP_DIR%sound_duty.png %TILEMAP_DIR%sound_envelope.png %TILEMAP_DIR%sound_frequency.png %TILEMAP_DIR%sound_length.png %TILEMAP_DIR%sound_sweep.png %TILEMAP_DIR%sound_volume.png %TILEMAP_DIR%timer_sec.png %TILEMAP_DIR%window.png
if %errorlevel% neq 0 goto end

%GFXCONV% %TILESET_DIR%parallax.png %TILEMAP_DIR%parallax.png
if %errorlevel% neq 0 goto end

%GFXCONV% %TILESET_DIR%sound_test.png %TILEMAP_DIR%ch1_test.png %TILEMAP_DIR%ch2_test.png %TILEMAP_DIR%ch3_test.png %TILEMAP_DIR%ch4_test.png
if %errorlevel% neq 0 goto end

del /f /s /q %CHR_DIR% 2> nul 1> nul
rmdir /s /q %CHR_DIR% 2> nul
mkdir %CHR_DIR% 2> nul
move %TILESET_DIR%*.%CHR_EXTENSION% %CHR_DIR%

del /f /s /q %TLM_DIR% 2> nul 1> nul
rmdir /s /q %TLM_DIR% 2> nul
mkdir %TLM_DIR% 2> nul
move %TILEMAP_DIR%*.%TLM_EXTENSION% %TLM_DIR%

:end
popd
exit /b %errorlevel%


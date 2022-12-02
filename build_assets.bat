echo off
cd "%~dp0"

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Common
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set BIN_DIR=bin\
set ASSETS_DIR=assets\
set GENERATED_DIR=generated\

set ATR_EXTENSION=atr
set CHR_EXTENSION=chr
set PAL_EXTENSION=pal
set TLM_EXTENSION=tlm
set TM1_EXTENSION=idx
set TM2_EXTENSION=prm
set PKT_EXTENSION=pkt

set PKT_CONV=%BIN_DIR%packetize.exe

set DMG_CONV=%BIN_DIR%gfxconv.exe
set CGB_CONV=%BIN_DIR%cgbconv.exe
set SGB_CONV=%BIN_DIR%sgbconv.exe

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: DMG
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set DMG_ASSETS_DIR=%ASSETS_DIR%dmg\
set DMG_TILESET_DIR=%DMG_ASSETS_DIR%tileset\
set DMG_TILEMAP_DIR=%DMG_ASSETS_DIR%tilemap\

set DMG_GENERATED_DIR=%GENERATED_DIR%dmg\
set DMG_CHR_DIR=%DMG_GENERATED_DIR%%CHR_EXTENSION%\
set DMG_TLM_DIR=%DMG_GENERATED_DIR%%TLM_EXTENSION%\

:: general samples
%DMG_CONV% %DMG_TILESET_DIR%tileset.png %DMG_TILEMAP_DIR%background.png %DMG_TILEMAP_DIR%ch1_continuous.png %DMG_TILEMAP_DIR%ch3_simple.png %DMG_TILEMAP_DIR%ch3_waveforms.png %DMG_TILEMAP_DIR%ch4_example.png %DMG_TILEMAP_DIR%counter_rng.png %DMG_TILEMAP_DIR%frame.png %DMG_TILEMAP_DIR%input_interrupt.png %DMG_TILEMAP_DIR%interrupt_trigger.png %DMG_TILEMAP_DIR%printer.png %DMG_TILEMAP_DIR%serial_multibytes.png %DMG_TILEMAP_DIR%serial_parent_child.png %DMG_TILEMAP_DIR%serial_switch_roles.png %DMG_TILEMAP_DIR%serial_transfer.png %DMG_TILEMAP_DIR%sgb_detection.png %DMG_TILEMAP_DIR%sound_duty.png %DMG_TILEMAP_DIR%sound_envelope.png %DMG_TILEMAP_DIR%sound_frequency.png %DMG_TILEMAP_DIR%sound_length.png %DMG_TILEMAP_DIR%sound_sweep.png %DMG_TILEMAP_DIR%sound_volume.png %DMG_TILEMAP_DIR%timer_sec.png %DMG_TILEMAP_DIR%window.png
if %errorlevel% neq 0 goto end

:: parallax sample
%DMG_CONV% %DMG_TILESET_DIR%parallax.png %DMG_TILEMAP_DIR%parallax.png
if %errorlevel% neq 0 goto end

:: sound test
%DMG_CONV% %DMG_TILESET_DIR%sound_test.png %DMG_TILEMAP_DIR%ch1_test.png %DMG_TILEMAP_DIR%ch2_test.png %DMG_TILEMAP_DIR%ch3_test.png %DMG_TILEMAP_DIR%ch4_test.png
if %errorlevel% neq 0 goto end

del /f /s /q %DMG_CHR_DIR% 2> nul 1> nul
rmdir /s /q %DMG_CHR_DIR% 2> nul
mkdir %DMG_CHR_DIR% 2> nul
move %DMG_TILESET_DIR%*.%CHR_EXTENSION% %DMG_CHR_DIR%

del /f /s /q %DMG_TLM_DIR% 2> nul 1> nul
rmdir /s /q %DMG_TLM_DIR% 2> nul
mkdir %DMG_TLM_DIR% 2> nul
move %DMG_TILEMAP_DIR%*.%TLM_EXTENSION% %DMG_TLM_DIR%

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: CGB
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set CGB_ASSETS_DIR=%ASSETS_DIR%cgb\

set CGB_GENERATED_DIR=%GENERATED_DIR%cgb\
set CGB_CHR_DIR=%CGB_GENERATED_DIR%%CHR_EXTENSION%\
set CGB_PAL_DIR=%CGB_GENERATED_DIR%%PAL_EXTENSION%\
set CGB_TLM_DIR=%CGB_GENERATED_DIR%%TLM_EXTENSION%\
set CGB_PKT_DIR=%CGB_GENERATED_DIR%%PKT_EXTENSION%\

%CGB_CONV% -mpt %CGB_ASSETS_DIR%ship.png %CGB_ASSETS_DIR%ship.png
if %errorlevel% neq 0 goto end
%CGB_CONV% %CGB_ASSETS_DIR%coins.png %CGB_ASSETS_DIR%coins.png
if %errorlevel% neq 0 goto end
%CGB_CONV% -mt %CGB_ASSETS_DIR%alphabet.png %CGB_ASSETS_DIR%cpu_speed_test.png
if %errorlevel% neq 0 goto end

:: printer sample
%CGB_CONV% %CGB_ASSETS_DIR%print_data.png
if %errorlevel% neq 0 goto end
%PKT_CONV% %CGB_ASSETS_DIR%print_data.chr
if %errorlevel% neq 0 goto end

del /f /s /q %CGB_CHR_DIR% 2> nul 1> nul
rmdir /s /q %CGB_CHR_DIR% 2> nul
mkdir %CGB_CHR_DIR% 2> nul
move %CGB_ASSETS_DIR%*.%CHR_EXTENSION% %CGB_CHR_DIR%

del /f /s /q %CGB_PAL_DIR% 2> nul 1> nul
rmdir /s /q %CGB_PAL_DIR% 2> nul
mkdir %CGB_PAL_DIR% 2> nul
move %CGB_ASSETS_DIR%*.%PAL_EXTENSION% %CGB_PAL_DIR%

del /f /s /q %CGB_TLM_DIR% 2> nul 1> nul
rmdir /s /q %CGB_TLM_DIR% 2> nul
mkdir %CGB_TLM_DIR% 2> nul
move %CGB_ASSETS_DIR%*.%TM1_EXTENSION% %CGB_TLM_DIR%
move %CGB_ASSETS_DIR%*.%TM2_EXTENSION% %CGB_TLM_DIR%

del /f /s /q %CGB_PKT_DIR% 2> nul 1> nul
rmdir /s /q %CGB_PKT_DIR% 2> nul
mkdir %CGB_PKT_DIR% 2> nul
move %CGB_ASSETS_DIR%*.%PKT_EXTENSION% %CGB_PKT_DIR%

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: SGB
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set SGB_ASSETS_DIR=%ASSETS_DIR%sgb\

set SGB_GENERATED_DIR=%GENERATED_DIR%sgb\
set SGB_CHR_DIR=%SGB_GENERATED_DIR%%CHR_EXTENSION%\
set SGB_PAL_DIR=%SGB_GENERATED_DIR%%PAL_EXTENSION%\
set SGB_TLM_DIR=%SGB_GENERATED_DIR%%TLM_EXTENSION%\
set SGB_ATR_DIR=%SGB_GENERATED_DIR%%ATR_EXTENSION%\

:: multi
%DMG_CONV% %SGB_ASSETS_DIR%tileset.png %SGB_ASSETS_DIR%sgb_multi.png %SGB_ASSETS_DIR%sgb_settings.png
if %errorlevel% neq 0 goto end

:: tech
%DMG_CONV% %SGB_ASSETS_DIR%tech_dmg_tileset.png %SGB_ASSETS_DIR%tech_dmg.png
if %errorlevel% neq 0 goto end
%SGB_CONV% %SGB_ASSETS_DIR%tech_sgb.png %SGB_ASSETS_DIR%tech_sgb.png
if %errorlevel% neq 0 goto end

:: sound test
%DMG_CONV% %SGB_ASSETS_DIR%sound_test.png %SGB_ASSETS_DIR%sgb_sound_test.png
if %errorlevel% neq 0 goto end

del /f /s /q %SGB_CHR_DIR% 2> nul 1> nul
rmdir /s /q %SGB_CHR_DIR% 2> nul
mkdir %SGB_CHR_DIR% 2> nul
move %SGB_ASSETS_DIR%*.%CHR_EXTENSION% %SGB_CHR_DIR%

del /f /s /q %SGB_PAL_DIR% 2> nul 1> nul
rmdir /s /q %SGB_PAL_DIR% 2> nul
mkdir %SGB_PAL_DIR% 2> nul
move %SGB_ASSETS_DIR%*.%PAL_EXTENSION% %SGB_PAL_DIR%

del /f /s /q %SGB_TLM_DIR% 2> nul 1> nul
rmdir /s /q %SGB_TLM_DIR% 2> nul
mkdir %SGB_TLM_DIR% 2> nul
move %SGB_ASSETS_DIR%*.%TLM_EXTENSION% %SGB_TLM_DIR%

del /f /s /q %SGB_ATR_DIR% 2> nul 1> nul
rmdir /s /q %SGB_ATR_DIR% 2> nul
mkdir %SGB_ATR_DIR% 2> nul
move %SGB_ASSETS_DIR%*.%ATR_EXTENSION% %SGB_ATR_DIR%

::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Exit
::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:end
popd
exit /b %errorlevel%


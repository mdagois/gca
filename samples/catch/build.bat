echo off
pushd "%~dp0"

for %%i in (.) do set rom_name=%%~ni

rgbasm -Werror -Weverything -Hl -o main.o main.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -Hl -o sample.o sample.rgbasm
if %errorlevel% neq 0 goto end
rgblink --pad 0xC7 --dmg --tiny --map %rom_name%.map --sym %rom_name%.sym -o %rom_name%.gb main.o sample.o
if %errorlevel% neq 0 goto end
rgbfix --title game --pad-value 0xC7 --validate %rom_name%.gb
if %errorlevel% neq 0 goto end

:end
popd
exit /b %errorlevel%


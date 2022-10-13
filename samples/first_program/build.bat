echo off
pushd "%~dp0"

for %%i in (.) do set rom_name=%%~ni

rgbasm -Werror -Weverything -Hl -o main.o main.rgbasm
if %errorlevel% neq 0 goto end
rgblink --dmg --tiny -o %rom_name%.gb main.o
if %errorlevel% neq 0 goto end
rgbfix --title game --pad-value 0 --validate %rom_name%.gb
if %errorlevel% neq 0 goto end

:end
popd
exit /b %errorlevel%


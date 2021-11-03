echo off
pushd "%~dp0"

for %%i in (.) do set rom_name=%%~ni

rgbasm -Werror -Weverything -o main.o main.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o sample.o sample.rgbasm
if %errorlevel% neq 0 goto end
rgblink --dmg --map %rom_name%.map --sym %rom_name%.sym -o %rom_name%.gb main.o sample.o
if %errorlevel% neq 0 goto end
rgbfix --title game --mbc-type 0x19 --pad-value 0 --validate %rom_name%.gb
if %errorlevel% neq 0 goto end

:end
popd
exit /b %errorlevel%


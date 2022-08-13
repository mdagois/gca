echo off
pushd "%~dp0"

for %%i in (.) do set rom_name=%%~ni

rgbasm -Werror -Weverything -o main.o main.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o data.o data.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o memory.o memory.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o printer.o printer.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o interrupt.o interrupt.rgbasm
if %errorlevel% neq 0 goto end
rgblink --dmg --tiny --map %rom_name%.map --sym %rom_name%.sym -o %rom_name%.gb main.o data.o interrupt.o memory.o printer.o
if %errorlevel% neq 0 goto end
rgbfix --title game --pad-value 0 --validate %rom_name%.gb
if %errorlevel% neq 0 goto end

:end
popd
exit /b %errorlevel%


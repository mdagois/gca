echo off
pushd "%~dp0"

for %%i in (.) do set rom_name=%%~ni

rgbasm -Werror -Weverything -o main.o main.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o sample.o sample.rgbasm
if %errorlevel% neq 0 goto end
rgblink --tiny --map %rom_name%.map --sym %rom_name%.sym -o %rom_name%.gbc main.o sample.o
if %errorlevel% neq 0 goto end
rgbfix --title game --color-compatible --pad-value 0 --validate %rom_name%.gbc
if %errorlevel% neq 0 goto end

:end
popd
exit /b %errorlevel%


echo off
pushd "%~dp0"

for %%i in (.) do set rom_name=%%~ni

rgbasm -Werror -Weverything -o main.o main.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o channel_data.o channel_data.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o shared.o shared.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o ch1.o ch1.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o ch2.o ch2.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o ch3.o ch3.rgbasm
if %errorlevel% neq 0 goto end
rgbasm -Werror -Weverything -o ch4.o ch4.rgbasm
if %errorlevel% neq 0 goto end
rgblink --dmg --tiny --map %rom_name%.map --sym %rom_name%.sym -o %rom_name%.gb main.o channel_data.o shared.o ch1.o ch2.o ch3.o ch4.o
if %errorlevel% neq 0 goto end
rgbfix --title game --pad-value 0 --validate %rom_name%.gb
if %errorlevel% neq 0 goto end

:end
popd
exit /b %errorlevel%


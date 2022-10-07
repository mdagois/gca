echo off
pushd "%~dp0"

set PORT=8765

for %%i in (.) do set rom_name=%%~ni

start /b bgb --watch --rom simulator\printer.gbc --listen %PORT%
start /b bgb --watch --rom %rom_name%.gb --connect 127.0.0.1:%PORT%

popd


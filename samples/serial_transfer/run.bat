echo off
pushd "%~dp0"

for %%i in (.) do set rom_name=%%~ni

start /b bgb --watch --rom %rom_name%.gb --listen 8765
start /b bgb --watch --rom %rom_name%.gb --connect 127.0.0.1

popd


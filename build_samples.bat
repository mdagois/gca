echo off
Setlocal enabledelayedexpansion

set SOURCE_DIR=samples
set ROMS_DIR=roms

set /a success = 0
set /a error = 0

pushd "%~dp0"
del /f /s /q %ROMS_DIR% 2> nul 1> nul
rmdir /s /q %ROMS_DIR% 2> nul
mkdir %ROMS_DIR% 2> nul
for /d %%d in (%SOURCE_DIR%\*) do (
	echo Building [%%~fd]...
	call %%~fd\build.bat
	if !errorlevel! equ 0 copy %%~fd\*.gb %ROMS_DIR% 1> nul && echo SUCCESS && set /a success += 1
	if !errorlevel! neq 0 echo ERROR && set /a error += 1
	echo.
)
popd

echo SUCCESS: %success%, ERROR: %error%
exit /b %errorlevel%


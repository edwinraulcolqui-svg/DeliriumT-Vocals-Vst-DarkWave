@echo off
setlocal
echo ============================================
echo   DELIRIUMT VOCALS - DARK GRAVE / DXTXM
echo   Windows x64 VST3 + Installer build
echo ============================================
echo.

where cmake >nul 2>nul || (echo ERROR: Install CMake 3.22+ first.& pause& exit /b 1)
where git >nul 2>nul || (echo ERROR: Install Git first.& pause& exit /b 1)

echo [1/3] Configuring x64 Release...
cmake -S . -B build -A x64
if errorlevel 1 goto fail

echo.
echo [2/3] Compiling VST3...
cmake --build build --config Release --target DeliriumTVocals_VST3
if errorlevel 1 goto fail

set "VST3=build\DeliriumTVocals_artefacts\Release\VST3\DeliriumT Vocals.vst3"
if not exist "%VST3%" (
  echo ERROR: VST3 bundle was not found at:
  echo %VST3%
  goto fail
)

echo.
echo [3/3] Creating Windows installer...
set "ISCC=C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
if not exist "%ISCC%" set "ISCC=C:\Program Files\Inno Setup 6\ISCC.exe"
if not exist "%ISCC%" (
  echo VST3 build succeeded, but Inno Setup 6 is not installed.
  echo Install Inno Setup 6, then run this file again to create the EXE.
  echo.
  echo VST3 location: %VST3%
  pause
  exit /b 0
)

"%ISCC%" "Installer\DeliriumT_Vocals.iss"
if errorlevel 1 goto fail

echo.
echo SUCCESS. Installer should be in:
echo Installer\Output\DeliriumT_Vocals_Windows_x64_Setup.exe
echo.
echo Install it, then open FL Studio and scan VST3 plugins.
pause
exit /b 0

:fail
echo.
echo BUILD FAILED. Review the error messages above.
pause
exit /b 1

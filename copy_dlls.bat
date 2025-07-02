@echo off
REM Auto-copy SDL2 DLL files for Windows distribution
echo [INFO] Copying SDL2 runtime libraries...

REM Check if MSYS2 exists
if not exist "C:\msys64\mingw64\bin\SDL2.dll" (
    echo [ERROR] MSYS2 SDL2 libraries not found at C:\msys64\mingw64\bin\
    echo Please ensure MSYS2 and SDL2 libraries are properly installed
    pause
    exit /b 1
)

echo [INFO] Copying main SDL2 libraries...
copy "C:\msys64\mingw64\bin\SDL2.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\SDL2_image.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\SDL2_ttf.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\SDL2_mixer.dll" . >nul 2>&1

echo [INFO] Copying dependency libraries...
copy "C:\msys64\mingw64\bin\libpng16-16.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libjpeg-8.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libfreetype-6.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libvorbis-0.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libvorbisfile-3.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libogg-0.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libgcc_s_seh-1.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libwinpthread-1.dll" . >nul 2>&1

REM SDL2_image additional dependencies
copy "C:\msys64\mingw64\bin\libavif-16.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libjxl.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libtiff-6.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libwebp-7.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libwebpmux-3.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libwebpdemux-2.dll" . >nul 2>&1

REM Additional common dependencies that might be needed
copy "C:\msys64\mingw64\bin\libbz2-1.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libzstd.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\liblzma-5.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libglib-2.0-0.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libintl-8.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libiconv-2.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libpcre2-8-0.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libharfbuzz-0.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libgraphite2.dll" . >nul 2>&1

REM More image format dependencies
copy "C:\msys64\mingw64\bin\libjpeg-8.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libdeflate.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libsharpyuv-0.dll" . >nul 2>&1
copy "C:\msys64\mingw64\bin\libLerc.dll" . >nul 2>&1

echo [INFO] Checking copied files...
set "missing_files="

if not exist "SDL2.dll" set "missing_files=SDL2.dll "
if not exist "SDL2_image.dll" set "missing_files=%missing_files%SDL2_image.dll "
if not exist "SDL2_ttf.dll" set "missing_files=%missing_files%SDL2_ttf.dll "
if not exist "SDL2_mixer.dll" set "missing_files=%missing_files%SDL2_mixer.dll "

if "%missing_files%"=="" (
    echo [SUCCESS] All main SDL2 libraries copied successfully!
    echo [INFO] You can now run knight_game.exe
    echo.
    echo [INFO] Files copied:
    dir *.dll /b
) else (
    echo [ERROR] Some files failed to copy: %missing_files%
    echo [INFO] You may need to run this script as administrator
)

echo.
echo Press any key to exit...
pause >nul 
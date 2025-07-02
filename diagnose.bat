@echo off
REM Diagnostic script for Super Knight Game on Windows
echo ========================================
echo Super Knight Game - Windows Diagnostics
echo ========================================
echo.

echo [STEP 1] Checking MSYS2 installation...
if exist "C:\msys64\mingw64\bin\gcc.exe" (
    echo [OK] MSYS2 found at C:\msys64\
) else (
    echo [ERROR] MSYS2 not found at C:\msys64\
    echo Please install MSYS2 from https://www.msys2.org/
    goto :end
)

echo.
echo [STEP 2] Checking current PATH...
echo Current PATH: %PATH%
echo %PATH% | findstr /i "msys64" >nul
if %errorlevel% equ 0 (
    echo [OK] MSYS2 found in PATH
) else (
    echo [WARNING] MSYS2 not in PATH, adding temporarily...
    set "PATH=C:\msys64\mingw64\bin;C:\msys64\usr\bin;%PATH%"
    set "PKG_CONFIG_PATH=C:\msys64\mingw64\lib\pkgconfig;C:\msys64\mingw64\share\pkgconfig"
)

echo.
echo [STEP 3] Checking GCC compiler...
where gcc >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] GCC found
    gcc --version | head -1
) else (
    echo [ERROR] GCC not found
    echo Run: pacman -S mingw-w64-x86_64-gcc
    goto :end
)

echo.
echo [STEP 4] Checking make tool...
where make >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] make found
    make --version | head -1
) else (
    echo [ERROR] make not found
    echo Run: pacman -S mingw-w64-x86_64-make
)

echo.
echo [STEP 5] Checking pkg-config...
where pkg-config >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] pkg-config found
    echo PKG_CONFIG_PATH: %PKG_CONFIG_PATH%
) else (
    echo [ERROR] pkg-config not found
    echo Run: pacman -S mingw-w64-x86_64-pkg-config
    goto :end
)

echo.
echo [STEP 6] Checking SDL2 libraries...
pkg-config --exists sdl2 >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] SDL2 found
    pkg-config --modversion sdl2
) else (
    echo [ERROR] SDL2 not found
    echo Run: pacman -S mingw-w64-x86_64-SDL2
)

pkg-config --exists SDL2_image >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] SDL2_image found
) else (
    echo [ERROR] SDL2_image not found
    echo Run: pacman -S mingw-w64-x86_64-SDL2_image
)

pkg-config --exists SDL2_ttf >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] SDL2_ttf found
) else (
    echo [ERROR] SDL2_ttf not found
    echo Run: pacman -S mingw-w64-x86_64-SDL2_ttf
)

pkg-config --exists SDL2_mixer >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] SDL2_mixer found
) else (
    echo [ERROR] SDL2_mixer not found
    echo Run: pacman -S mingw-w64-x86_64-SDL2_mixer
)

echo.
echo [STEP 7] Testing SDL2 compile flags...
for /f "delims=" %%i in ('pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer 2^>nul') do set "SDL_FLAGS=%%i"
if "%SDL_FLAGS%"=="" (
    echo [ERROR] Failed to get SDL2 compile flags
) else (
    echo [OK] SDL2 compile flags: %SDL_FLAGS%
)

echo.
echo [STEP 8] Checking source files...
if exist "scripts\main.c" (
    echo [OK] Source files found
) else (
    echo [ERROR] Source files not found in scripts\ directory
    echo Make sure you're running this from the project root
)

echo.
echo ========================================
echo Diagnosis complete!
echo ========================================

:end
echo.
echo Press any key to exit...
pause >nul 
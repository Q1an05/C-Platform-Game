@echo off
REM Simple Windows Build Script for Super Knight Game
REM No Chinese characters, maximum compatibility

echo [INFO] Super Knight Game Build Script
echo.

REM Check if MSYS2 is in PATH
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] GCC not found in PATH
    echo [INFO] Please ensure MSYS2 is installed and added to PATH
    echo [INFO] Expected location: C:\msys64\mingw64\bin
    if exist "C:\msys64\mingw64\bin\gcc.exe" (
        echo [INFO] Found MSYS2, adding to PATH...
        set "PATH=C:\msys64\mingw64\bin;C:\msys64\usr\bin;%PATH%"
        set "PKG_CONFIG_PATH=C:\msys64\mingw64\lib\pkgconfig;C:\msys64\mingw64\share\pkgconfig"
    ) else (
        echo [ERROR] Please install MSYS2 from https://www.msys2.org/
        pause
        exit /b 1
    )
)

REM Check pkg-config
where pkg-config >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] pkg-config not found
    echo [INFO] Run: pacman -S mingw-w64-x86_64-pkg-config
    pause
    exit /b 1
)

REM Check SDL2
pkg-config --exists sdl2 >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] SDL2 not found
    echo [INFO] Run: pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
    pause
    exit /b 1
)

echo [INFO] All dependencies found
echo [INFO] Starting compilation...

REM Compile
gcc -std=c99 -Wall -o knight_game.exe scripts\main.c scripts\knight.c scripts\map.c scripts\render.c scripts\input.c scripts\camera.c scripts\blocks.c scripts\enemy.c scripts\ui.c scripts\sound.c -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

if %errorlevel% equ 0 (
    echo [SUCCESS] Compilation successful!
    echo [INFO] Run the game: knight_game.exe
    if "%1"=="run" (
        echo [INFO] Starting game...
        knight_game.exe
    )
) else (
    echo [ERROR] Compilation failed!
    pause
    exit /b 1
)

echo [INFO] Build script completed
pause 
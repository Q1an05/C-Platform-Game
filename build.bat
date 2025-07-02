@echo off
REM Super Knight Game Windows Build Script
REM Encoding: ANSI to avoid Chinese character issues

REM Color definitions for Windows console
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

REM Print colored messages
goto :main

:print_info
echo %BLUE%[INFO]%NC% %~1
goto :eof

:print_success
echo %GREEN%[SUCCESS]%NC% %~1
goto :eof

:print_warning
echo %YELLOW%[WARNING]%NC% %~1
goto :eof

:print_error
echo %RED%[ERROR]%NC% %~1
goto :eof

REM Check if we're in MSYS2 environment
:check_environment
call :print_info "Checking environment..."

REM Check if we're in MSYS2/MinGW environment
echo %PATH% | findstr /i "msys64" >nul
if %errorlevel% equ 0 (
    call :print_info "MSYS2 environment detected"
    goto :check_dependencies
)

REM Check if we're in regular Windows CMD
call :print_warning "Regular Windows CMD detected"
call :print_info "Checking for MSYS2 installation..."

if exist "C:\msys64\mingw64\bin\gcc.exe" (
    call :print_info "MSYS2 found, adding to PATH..."
    set "PATH=C:\msys64\mingw64\bin;C:\msys64\usr\bin;%PATH%"
    set "PKG_CONFIG_PATH=C:\msys64\mingw64\lib\pkgconfig;C:\msys64\mingw64\share\pkgconfig"
    call :print_info "PATH updated: %PATH%"
    call :print_info "PKG_CONFIG_PATH set: %PKG_CONFIG_PATH%"
) else (
    call :print_error "MSYS2 not found in C:\msys64\"
    call :print_info "Please install MSYS2 from https://www.msys2.org/"
    call :print_info "Or run this script from MSYS2 MINGW64 terminal"
    echo.
    echo Press any key to exit...
    pause >nul
    exit /b 1
)
goto :check_dependencies

REM Check dependencies
:check_dependencies
call :print_info "Checking dependencies..."

REM Check GCC compiler
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    call :print_error "GCC compiler not found"
    call :print_info "Solution: pacman -S mingw-w64-x86_64-gcc"
    echo.
    echo Press any key to exit...
    pause >nul
    exit /b 1
)

REM Check make tool
where make >nul 2>&1
if %errorlevel% neq 0 (
    call :print_error "make tool not found"
    call :print_info "Solution: pacman -S mingw-w64-x86_64-make"
    exit /b 1
)

REM Check pkg-config tool
where pkg-config >nul 2>&1
if %errorlevel% neq 0 (
    call :print_error "pkg-config tool not found"
    call :print_info "Solution: pacman -S mingw-w64-x86_64-pkg-config"
    echo.
    echo Press any key to exit...
    pause >nul
    exit /b 1
)

REM Check SDL2 libraries
pkg-config --exists sdl2 >nul 2>&1
if %errorlevel% neq 0 (
    call :print_error "SDL2 not found or PKG_CONFIG_PATH not set"
    call :print_info "Current PKG_CONFIG_PATH: %PKG_CONFIG_PATH%"
    call :print_info "Solutions:"
    call :print_info "1. pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer"
    call :print_info "2. Set PKG_CONFIG_PATH environment variable"
    echo.
    echo Press any key to exit...
    pause >nul
    exit /b 1
)

REM Check individual SDL2 components
pkg-config --exists SDL2_image >nul 2>&1
if %errorlevel% neq 0 (
    call :print_warning "SDL2_image may not be installed correctly"
)

pkg-config --exists SDL2_ttf >nul 2>&1
if %errorlevel% neq 0 (
    call :print_warning "SDL2_ttf may not be installed correctly"
)

pkg-config --exists SDL2_mixer >nul 2>&1
if %errorlevel% neq 0 (
    call :print_warning "SDL2_mixer may not be installed correctly"
)

call :print_success "Dependencies check completed"
goto :eof

REM Clean build files
:clean
call :print_info "Cleaning build files..."
if exist knight_game.exe del knight_game.exe
if exist *.o del *.o
if exist *.obj del *.obj
call :print_success "Clean completed"
goto :eof

REM Compile game
:compile
call :print_info "Compiling Knight Game..."

REM Compilation parameters
set "CC=gcc"
set "CFLAGS=-std=c99 -Wall"
set "SOURCES=scripts\main.c scripts\knight.c scripts\map.c scripts\render.c scripts\input.c scripts\camera.c scripts\blocks.c scripts\enemy.c scripts\ui.c scripts\sound.c"
set "TARGET=knight_game.exe"

REM Get SDL2 compilation flags
for /f "delims=" %%i in ('pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer') do set "LDFLAGS=%%i"

REM Show compilation command
call :print_info "Compile command: %CC% %CFLAGS% %LDFLAGS% -o %TARGET% %SOURCES%"

REM Execute compilation
%CC% %CFLAGS% %LDFLAGS% -o %TARGET% %SOURCES%

REM Check compilation result
if %errorlevel% equ 0 (
    call :print_success "Compilation successful!"
    if exist "%TARGET%" (
        dir "%TARGET%"
        call :print_info "Run game: %TARGET%"
    )
    exit /b 0
) else (
    call :print_error "Compilation failed!"
    exit /b 1
)

REM Run game
:run
if not exist "knight_game.exe" (
    call :print_warning "Game not compiled, compiling now..."
    call :compile
    if %errorlevel% neq 0 exit /b 1
)

call :print_info "Starting game..."
knight_game.exe
goto :eof

REM Install dependencies guide
:install_deps
call :print_info "Windows SDL2 Dependencies Installation Guide..."
echo.
echo ========== RECOMMENDED SOLUTION (MSYS2) ==========
echo 1. Install MSYS2:
echo    Download: https://www.msys2.org/
echo    After installation, open MSYS2 MINGW64 terminal
echo.
echo 2. Install all dependencies at once:
echo    pacman -Syu
echo    pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
echo.
echo 3. Set environment variables:
echo    export PATH="/mingw64/bin:/usr/bin:$PATH"
echo    export PKG_CONFIG_PATH="/mingw64/lib/pkgconfig:/mingw64/share/pkgconfig"
echo.
echo 4. Verify installation:
echo    which gcc ^&^& which make ^&^& which pkg-config
echo    pkg-config --cflags --libs sdl2
echo.
echo ========== COMMON ERROR FIXES ==========
echo Error: 'make' is not recognized as an internal or external command
echo Solution: pacman -S mingw-w64-x86_64-make
echo.
echo Error: Package 'sdl2' was not found
echo Solution: pacman -S mingw-w64-x86_64-pkg-config
echo           export PKG_CONFIG_PATH="/mingw64/lib/pkgconfig:/mingw64/share/pkgconfig"
echo.
echo For detailed documentation: see WINDOWS_SUPPORT.md
goto :eof

REM Show help
:show_help
echo %BLUE%Super Knight Game Windows Build Script%NC%
echo.
echo Usage: %~nx0 [option]
echo.
echo Options:
echo   build, compile    Compile the game
echo   run              Compile and run the game
echo   clean            Clean build files
echo   deps, install    Show dependency installation guide
echo   help, -h         Show help information
echo.
echo Examples:
echo   %~nx0                # Default: compile game
echo   %~nx0 run           # Compile and run
echo   %~nx0 clean         # Clean files
echo.
echo Note: SDL2 related libraries must be installed first
goto :eof

REM Main logic
:main
set "ACTION=%~1"
if "%ACTION%"=="" set "ACTION=build"

if "%ACTION%"=="build" goto :action_build
if "%ACTION%"=="compile" goto :action_build
if "%ACTION%"=="run" goto :action_run
if "%ACTION%"=="clean" goto :action_clean
if "%ACTION%"=="deps" goto :action_deps
if "%ACTION%"=="install" goto :action_deps
if "%ACTION%"=="help" goto :action_help
if "%ACTION%"=="-h" goto :action_help
if "%ACTION%"=="--help" goto :action_help

call :print_error "Unknown option: %ACTION%"
call :show_help
exit /b 1

:action_build
call :check_environment
if %errorlevel% neq 0 exit /b 1
call :compile
exit /b %errorlevel%

:action_run
call :check_environment
if %errorlevel% neq 0 exit /b 1
call :run
exit /b 0

:action_clean
call :clean
exit /b 0

:action_deps
call :install_deps
exit /b 0

:action_help
call :show_help
exit /b 0 
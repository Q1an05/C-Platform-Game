@echo off
REM Smart DLL dependency finder for Windows
echo [INFO] Checking for missing DLL dependencies...

if not exist "knight_game.exe" (
    echo [ERROR] knight_game.exe not found. Please compile first.
    pause
    exit /b 1
)

echo [INFO] Running dependency check...
echo [INFO] This will show all missing DLL files when trying to run the game

REM Try to run the game and capture error output
echo [INFO] Attempting to run knight_game.exe to detect missing DLLs...
echo [INFO] (The game window may appear briefly and close - this is normal)

REM Create a temporary batch file to capture the error
echo @echo off > temp_run.bat
echo knight_game.exe >> temp_run.bat
echo echo Error code: %%errorlevel%% >> temp_run.bat

REM Run and capture output
call temp_run.bat 2>error_output.txt

echo.
echo [INFO] If you see error messages about missing DLL files above,
echo [INFO] here are the most common missing DLLs and how to fix them:
echo.

REM List of all possible DLL files that might be needed
set "DLL_LIST=SDL2.dll SDL2_image.dll SDL2_ttf.dll SDL2_mixer.dll libpng16-16.dll libjpeg-8.dll libfreetype-6.dll libvorbis-0.dll libvorbisfile-3.dll libogg-0.dll libgcc_s_seh-1.dll libwinpthread-1.dll libavif-16.dll libjxl.dll libtiff-6.dll libwebp-7.dll libwebpmux-3.dll libwebpdemux-2.dll libbz2-1.dll libzstd.dll liblzma-5.dll libglib-2.0-0.dll libintl-8.dll libiconv-2.dll libpcre2-8-0.dll libharfbuzz-0.dll libgraphite2.dll libdeflate.dll libsharpyuv-0.dll libLerc.dll"

echo [INFO] Checking which DLLs are missing in current directory:
for %%d in (%DLL_LIST%) do (
    if not exist "%%d" (
        if exist "C:\msys64\mingw64\bin\%%d" (
            echo [MISSING] %%d - Available in MSYS2
        ) else (
            echo [MISSING] %%d - Not found in MSYS2
        )
    ) else (
        echo [OK] %%d - Present
    )
)

echo.
echo [INFO] To copy all missing DLLs, run: copy_dlls.bat
echo [INFO] To copy individual DLLs manually:
echo [INFO] copy "C:\msys64\mingw64\bin\[DLL_NAME]" .

REM Clean up
if exist temp_run.bat del temp_run.bat
if exist error_output.txt del error_output.txt

echo.
echo Press any key to exit...
pause >nul 
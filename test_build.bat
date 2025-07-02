@echo off
REM Test version of build script to debug issues
echo [TEST] Testing build script functionality...

echo [TEST] Checking if we can call functions...
call :test_function
echo [TEST] Function call successful

echo [TEST] Checking environment variables...
echo PATH contains: %PATH%

echo [TEST] Checking MSYS2 detection...
echo %PATH% | findstr /i "msys64" >nul
if %errorlevel% equ 0 (
    echo [TEST] MSYS2 found in PATH
) else (
    echo [TEST] MSYS2 not in PATH, checking installation...
    if exist "C:\msys64\mingw64\bin\gcc.exe" (
        echo [TEST] MSYS2 installation found
    ) else (
        echo [TEST] MSYS2 not found
    )
)

echo [TEST] Testing completed successfully
echo.
echo Press any key to exit...
pause >nul
exit /b 0

:test_function
echo [TEST] Inside test function
goto :eof 
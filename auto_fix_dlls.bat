@echo off
REM 智能DLL依赖检测和自动修复脚本
setlocal enabledelayedexpansion

echo [信息] 智能DLL依赖检测和修复工具
echo [信息] 正在检测knight_game.exe的所有依赖...

if not exist "knight_game.exe" (
    echo [错误] 找不到knight_game.exe，请先编译游戏
    pause
    exit /b 1
)

if not exist "C:\msys64\mingw64\bin" (
    echo [错误] 找不到MSYS2安装目录
    echo [信息] 请确保MSYS2安装在 C:\msys64
    pause
    exit /b 1
)

REM 创建临时目录存储检测结果
if not exist "temp_dll_check" mkdir temp_dll_check

echo [信息] 使用objdump检测依赖关系...

REM 使用objdump获取所有依赖的DLL
"C:\msys64\mingw64\bin\objdump.exe" -p knight_game.exe | findstr "DLL Name:" > temp_dll_check\dependencies.txt

if %errorlevel% neq 0 (
    echo [警告] objdump检测失败，使用备用方法...
    goto FALLBACK_METHOD
)

echo [信息] 检测到的依赖关系：
type temp_dll_check\dependencies.txt

echo.
echo [信息] 正在复制缺失的DLL文件...

set "copied_count=0"
set "missing_count=0"

REM 解析依赖文件并复制DLL
for /f "tokens=3" %%a in (temp_dll_check\dependencies.txt) do (
    set "dll_name=%%a"
    if not exist "!dll_name!" (
        echo [检查] !dll_name!
        if exist "C:\msys64\mingw64\bin\!dll_name!" (
            copy "C:\msys64\mingw64\bin\!dll_name!" . >nul 2>&1
            if !errorlevel! equ 0 (
                echo [复制] !dll_name! - 成功
                set /a copied_count+=1
            ) else (
                echo [失败] !dll_name! - 复制失败
            )
        ) else (
            echo [缺失] !dll_name! - MSYS2中未找到
            set /a missing_count+=1
        )
    ) else (
        echo [存在] !dll_name! - 已存在
    )
)

goto RECURSIVE_CHECK

:FALLBACK_METHOD
echo [信息] 使用预定义DLL列表进行检测...

REM 预定义的常用DLL列表
set "common_dlls=SDL2.dll SDL2_image.dll SDL2_mixer.dll SDL2_ttf.dll libgcc_s_seh-1.dll libwinpthread-1.dll libstdc++-6.dll libpng16-16.dll libjpeg-8.dll libtiff-6.dll libwebp-7.dll libavif-16.dll libjxl.dll libzstd.dll zlib1.dll liblzma-5.dll libdeflate.dll libsharpyuv-0.dll libLerc.dll libbrotlidec.dll libbrotlicommon.dll libbrotlienc.dll libhwy.dll libjxl_threads.dll libdav1d.dll librav1e.dll libSvtAv1Enc.dll libaom.dll libheif.dll libde265.dll libx265.dll libgomp-1.dll libvorbis-0.dll libvorbisfile-3.dll libogg-0.dll libfreetype-6.dll"

for %%d in (%common_dlls%) do (
    if not exist "%%d" (
        if exist "C:\msys64\mingw64\bin\%%d" (
            copy "C:\msys64\mingw64\bin\%%d" . >nul 2>&1
            if !errorlevel! equ 0 (
                echo [复制] %%d - 成功
                set /a copied_count+=1
            )
        ) else (
            echo [缺失] %%d - MSYS2中未找到
            set /a missing_count+=1
        )
    )
)

:RECURSIVE_CHECK
echo.
echo [信息] 进行递归依赖检测...

REM 递归检测新复制的DLL的依赖
set "new_dlls_found=1"
set "iteration=1"

:RECURSIVE_LOOP
if !iteration! gtr 5 goto END_RECURSIVE
if !new_dlls_found! equ 0 goto END_RECURSIVE

echo [信息] 第 !iteration! 轮递归检测...
set "new_dlls_found=0"

for %%f in (*.dll) do (
    if exist "C:\msys64\mingw64\bin\objdump.exe" (
        "C:\msys64\mingw64\bin\objdump.exe" -p "%%f" 2>nul | findstr "DLL Name:" > temp_dll_check\%%f_deps.txt
        
        for /f "tokens=3" %%a in (temp_dll_check\%%f_deps.txt) do (
            set "dll_name=%%a"
            if not exist "!dll_name!" (
                if exist "C:\msys64\mingw64\bin\!dll_name!" (
                    copy "C:\msys64\mingw64\bin\!dll_name!" . >nul 2>&1
                    if !errorlevel! equ 0 (
                        echo [递归复制] !dll_name! - 成功
                        set /a copied_count+=1
                        set "new_dlls_found=1"
                    )
                )
            )
        )
    )
)

set /a iteration+=1
goto RECURSIVE_LOOP

:END_RECURSIVE

echo.
echo [信息] 测试游戏是否可以运行...

REM 测试运行游戏
timeout /t 1 >nul
knight_game.exe --version >nul 2>&1
set "test_result=%errorlevel%"

if %test_result% equ 0 (
    echo [成功] 游戏可以正常启动！
) else (
    echo [警告] 游戏启动测试失败，可能还有依赖问题
    echo [信息] 尝试直接运行游戏查看具体错误信息
)

echo.
echo [总结] 复制了 !copied_count! 个DLL文件
if !missing_count! gtr 0 (
    echo [警告] 有 !missing_count! 个DLL在MSYS2中未找到
)

REM 清理临时文件
if exist "temp_dll_check" rmdir /s /q temp_dll_check

echo.
echo [信息] DLL修复完成！
echo [信息] 如果游戏仍然无法运行，请直接运行knight_game.exe查看错误信息
echo.
pause 
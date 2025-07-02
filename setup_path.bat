@echo off
REM Windows PATH环境变量配置脚本
REM 用于配置SDL2运行时库路径，替代复制dll方式

echo [信息] 开始配置Windows PATH环境变量...
echo [信息] 这将配置SDL2运行时库路径，无需复制dll文件
echo.

REM 检查MSYS2安装
if not exist "C:\msys64\mingw64\bin" (
    echo [错误] 找不到MSYS2安装目录 C:\msys64\mingw64\bin
    echo [信息] 请安装MSYS2到默认位置：https://www.msys2.org/
    pause
    exit /b 1
)

REM 检查必要的SDL2库文件
echo [信息] 检查SDL2库文件...
set "msys_bin=C:\msys64\mingw64\bin"
set "missing_libs="

if not exist "%msys_bin%\SDL2.dll" set "missing_libs=%missing_libs% SDL2.dll"
if not exist "%msys_bin%\SDL2_image.dll" set "missing_libs=%missing_libs% SDL2_image.dll"
if not exist "%msys_bin%\SDL2_ttf.dll" set "missing_libs=%missing_libs% SDL2_ttf.dll"
if not exist "%msys_bin%\SDL2_mixer.dll" set "missing_libs=%missing_libs% SDL2_mixer.dll"

if not "%missing_libs%"=="" (
    echo [错误] 缺少必要的SDL2库文件：%missing_libs%
    echo [信息] 请在MSYS2 MINGW64终端运行：
    echo pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
    pause
    exit /b 1
)

echo [成功] 所有必要的SDL2库文件都已找到

REM 检查当前PATH设置
echo.
echo [信息] 检查当前PATH设置...
echo %PATH% | findstr /i "msys64\\mingw64\\bin" >nul
if %errorlevel% equ 0 (
    echo [信息] MSYS2已在当前会话PATH中
) else (
    echo [信息] MSYS2不在当前会话PATH中，将临时添加
)

REM 为当前会话设置PATH
echo [信息] 为当前会话配置PATH环境变量...
set "PATH=C:\msys64\mingw64\bin;C:\msys64\usr\bin;%PATH%"
set "PKG_CONFIG_PATH=C:\msys64\mingw64\lib\pkgconfig;C:\msys64\mingw64\share\pkgconfig"

echo [成功] 当前会话PATH已配置

REM 检查是否需要永久设置PATH
echo.
echo [选项] 是否要永久设置PATH环境变量？
echo [信息] 这将修改系统环境变量，使所有新打开的命令提示符都能找到SDL2库
echo [警告] 需要管理员权限来修改系统环境变量
echo.
echo 选择操作：
echo 1. 仅为当前会话设置PATH（推荐，临时生效）
echo 2. 永久设置用户PATH环境变量
echo 3. 永久设置系统PATH环境变量（需要管理员权限）
echo 4. 显示手动设置PATH的说明
echo.
set /p choice="请输入选择 (1-4): "

if "%choice%"=="1" goto SESSION_ONLY
if "%choice%"=="2" goto USER_PATH
if "%choice%"=="3" goto SYSTEM_PATH
if "%choice%"=="4" goto MANUAL_HELP
goto SESSION_ONLY

:SESSION_ONLY
echo [信息] 已为当前会话配置PATH，游戏可以正常运行
echo [注意] 关闭命令提示符后设置将失效
goto VERIFY

:USER_PATH
echo [信息] 正在设置用户PATH环境变量...
setx PATH "C:\msys64\mingw64\bin;%PATH%"
if %errorlevel% equ 0 (
    echo [成功] 用户PATH环境变量已更新
    echo [注意] 新设置将在重新打开命令提示符后生效
) else (
    echo [错误] 设置用户PATH失败
)
goto VERIFY

:SYSTEM_PATH
echo [信息] 正在设置系统PATH环境变量...
echo [警告] 此操作需要管理员权限
setx PATH "C:\msys64\mingw64\bin;%PATH%" /m
if %errorlevel% equ 0 (
    echo [成功] 系统PATH环境变量已更新
    echo [注意] 新设置将在重新打开命令提示符后生效
) else (
    echo [错误] 设置系统PATH失败，可能需要管理员权限
    echo [信息] 请以管理员身份运行此脚本，或选择用户PATH选项
)
goto VERIFY

:MANUAL_HELP
echo [手动设置PATH说明]
echo.
echo 1. 右键点击"此电脑" -> "属性"
echo 2. 点击"高级系统设置"
echo 3. 点击"环境变量"
echo 4. 在用户变量或系统变量中找到"PATH"
echo 5. 编辑PATH，添加以下路径：
echo    C:\msys64\mingw64\bin
echo    C:\msys64\usr\bin
echo 6. 确定保存，重新打开命令提示符
echo.
pause
goto SESSION_ONLY

:VERIFY
echo.
echo [信息] 验证PATH配置...

REM 测试SDL2库是否可以找到
where SDL2.dll >nul 2>&1
if %errorlevel% equ 0 (
    echo [成功] SDL2.dll 已在PATH中找到
    for /f "tokens=*" %%i in ('where SDL2.dll 2^>nul') do echo [位置] %%i
) else (
    echo [警告] SDL2.dll 未在PATH中找到
)

where gcc >nul 2>&1
if %errorlevel% equ 0 (
    echo [成功] GCC编译器已在PATH中找到
) else (
    echo [警告] GCC编译器未在PATH中找到
)

echo.
echo [信息] PATH配置完成！
echo [使用说明]
echo 1. 现在可以直接运行 knight_game.exe，无需复制dll文件
echo 2. 如果选择了临时设置，关闭命令提示符后需要重新运行此脚本
echo 3. 如果选择了永久设置，重新打开命令提示符后即可使用
echo.
echo [测试] 如果游戏已编译，现在可以测试运行：
if exist "knight_game.exe" (
    echo knight_game.exe
) else (
    echo 请先编译游戏：build_simple.bat
)

echo.
echo 按任意键退出...
pause >nul 
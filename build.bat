@echo off
REM 超级骑士游戏Windows编译脚本
chcp 65001 >nul

REM 颜色定义（Windows控制台）
set "RED=[91m"
set "GREEN=[92m"
set "YELLOW=[93m"
set "BLUE=[94m"
set "NC=[0m"

REM 打印带颜色的消息
goto :main

:print_info
echo %BLUE%[信息]%NC% %~1
goto :eof

:print_success
echo %GREEN%[成功]%NC% %~1
goto :eof

:print_warning
echo %YELLOW%[警告]%NC% %~1
goto :eof

:print_error
echo %RED%[错误]%NC% %~1
goto :eof

REM 检查依赖
:check_dependencies
call :print_info "检查依赖..."

REM 检查编译器
where gcc >nul 2>&1
if %errorlevel% neq 0 (
    call :print_error "GCC编译器未找到"
    call :print_info "请安装MinGW-w64或MSYS2"
    call :print_info "推荐安装方式:"
    call :print_info "1. 安装MSYS2: https://www.msys2.org/"
    call :print_info "2. 运行: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer"
    exit /b 1
)

REM 检查pkg-config（通过SDL2库检查）
pkg-config --exists sdl2 >nul 2>&1
if %errorlevel% neq 0 (
    call :print_error "SDL2未正确安装或pkg-config配置错误"
    call :print_info "请确保SDL2相关库已正确安装"
    call :print_info "MSYS2安装命令:"
    call :print_info "pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer"
    exit /b 1
)

REM 检查各个SDL2组件
pkg-config --exists SDL2_image >nul 2>&1
if %errorlevel% neq 0 (
    call :print_warning "SDL2_image可能未正确安装"
)

pkg-config --exists SDL2_ttf >nul 2>&1
if %errorlevel% neq 0 (
    call :print_warning "SDL2_ttf可能未正确安装"
)

pkg-config --exists SDL2_mixer >nul 2>&1
if %errorlevel% neq 0 (
    call :print_warning "SDL2_mixer可能未正确安装"
)

call :print_success "依赖检查完成"
goto :eof

REM 清理编译文件
:clean
call :print_info "清理编译文件..."
if exist knight_game.exe del knight_game.exe
if exist *.o del *.o
if exist *.obj del *.obj
call :print_success "清理完成"
goto :eof

REM 编译游戏
:compile
call :print_info "编译骑士游戏..."

REM 编译参数
set "CC=gcc"
set "CFLAGS=-std=c99 -Wall"
set "SOURCES=scripts\main.c scripts\knight.c scripts\map.c scripts\render.c scripts\input.c scripts\camera.c scripts\blocks.c scripts\enemy.c scripts\ui.c scripts\sound.c"
set "TARGET=knight_game.exe"

REM 获取SDL2编译参数
for /f "delims=" %%i in ('pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer') do set "LDFLAGS=%%i"

REM 显示编译命令
call :print_info "编译命令: %CC% %CFLAGS% %LDFLAGS% -o %TARGET% %SOURCES%"

REM 执行编译
%CC% %CFLAGS% %LDFLAGS% -o %TARGET% %SOURCES%

REM 检查编译结果
if %errorlevel% equ 0 (
    call :print_success "编译成功！"
    if exist "%TARGET%" (
        dir "%TARGET%"
        call :print_info "运行游戏: %TARGET%"
    )
    exit /b 0
) else (
    call :print_error "编译失败！"
    exit /b 1
)

REM 运行游戏
:run
if not exist "knight_game.exe" (
    call :print_warning "游戏未编译，正在编译..."
    call :compile
    if %errorlevel% neq 0 exit /b 1
)

call :print_info "启动游戏..."
knight_game.exe
goto :eof

REM 安装依赖（指导用户）
:install_deps
call :print_info "Windows SDL2依赖安装指南..."
echo.
echo 推荐安装方式1 - MSYS2 (推荐):
echo 1. 下载并安装MSYS2: https://www.msys2.org/
echo 2. 打开MSYS2 MINGW64终端
echo 3. 更新包数据库: pacman -Syu
echo 4. 安装依赖: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
echo 5. 将MSYS2的mingw64/bin目录添加到系统PATH
echo.
echo 推荐安装方式2 - vcpkg:
echo 1. 安装vcpkg: git clone https://github.com/Microsoft/vcpkg.git
echo 2. 运行: .\vcpkg\bootstrap-vcpkg.bat
echo 3. 安装SDL2: .\vcpkg\vcpkg install sdl2 sdl2-image sdl2-ttf sdl2-mixer
echo.
echo 推荐安装方式3 - 预编译库:
echo 1. 从SDL官网下载开发库: https://www.libsdl.org/
echo 2. 解压到项目目录并配置环境变量
echo.
goto :eof

REM 显示帮助
:show_help
echo %BLUE%超级骑士游戏Windows编译脚本%NC%
echo.
echo 用法: %~nx0 [选项]
echo.
echo 选项:
echo   build, compile    编译游戏
echo   run              编译并运行游戏
echo   clean            清理编译文件
echo   deps, install    显示依赖安装指南
echo   help, -h         显示帮助信息
echo.
echo 示例:
echo   %~nx0                # 默认编译游戏
echo   %~nx0 run           # 编译并运行
echo   %~nx0 clean         # 清理文件
echo.
echo 注意: 需要先安装SDL2相关依赖库
goto :eof

REM 主逻辑
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

call :print_error "未知选项: %ACTION%"
call :show_help
exit /b 1

:action_build
call :check_dependencies
if %errorlevel% neq 0 exit /b 1
call :compile
exit /b %errorlevel%

:action_run
call :check_dependencies
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
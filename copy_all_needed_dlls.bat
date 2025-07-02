@echo off
REM 复制所有可能需要的DLL文件
echo [信息] 正在复制所有SDL2相关的DLL文件...

if not exist "C:\msys64\mingw64\bin" (
    echo [错误] 找不到MSYS2目录，请确保安装在 C:\msys64
    pause
    exit /b 1
)

set "msys_bin=C:\msys64\mingw64\bin"
set "copied=0"

REM 定义所有可能需要的DLL列表
set dll_list=^
SDL2.dll ^
SDL2_image.dll ^
SDL2_mixer.dll ^
SDL2_ttf.dll ^
libgcc_s_seh-1.dll ^
libwinpthread-1.dll ^
libstdc++-6.dll ^
libpng16-16.dll ^
libjpeg-8.dll ^
libtiff-6.dll ^
libwebp-7.dll ^
libwebpmux-3.dll ^
libwebpdemux-2.dll ^
libavif-16.dll ^
libjxl.dll ^
libjxl_threads.dll ^
libzstd.dll ^
zlib1.dll ^
liblzma-5.dll ^
libbz2-1.dll ^
libdeflate.dll ^
libsharpyuv-0.dll ^
libLerc.dll ^
libbrotlidec.dll ^
libbrotlicommon.dll ^
libbrotlienc.dll ^
libhwy.dll ^
libdav1d.dll ^
librav1e.dll ^
libSvtAv1Enc.dll ^
libaom.dll ^
libheif.dll ^
libde265.dll ^
libx265.dll ^
libgomp-1.dll ^
libvorbis-0.dll ^
libvorbisfile-3.dll ^
libogg-0.dll ^
libfreetype-6.dll ^
libharfbuzz-0.dll ^
libgraphite2.dll ^
libglib-2.0-0.dll ^
libintl-8.dll ^
libiconv-2.dll ^
libpcre2-8-0.dll ^
libffi-8.dll ^
libssp-0.dll

echo [信息] 开始复制DLL文件...

for %%d in (%dll_list%) do (
    if exist "%msys_bin%\%%d" (
        if not exist "%%d" (
            copy "%msys_bin%\%%d" . >nul 2>&1
            if !errorlevel! equ 0 (
                echo [复制] %%d
                set /a copied+=1
            ) else (
                echo [失败] %%d
            )
        ) else (
            echo [存在] %%d
        )
    ) else (
        echo [跳过] %%d - 源文件不存在
    )
)

echo.
echo [完成] 共复制了 %copied% 个DLL文件
echo [信息] 现在可以尝试运行游戏了

echo.
pause 
#!/bin/bash
# 简单的编译脚本

echo "编译超级玛丽游戏..."

# 检查SDL2是否安装
if ! command -v sdl2-config &> /dev/null; then
    echo "错误：SDL2未安装。请先运行: brew install sdl2"
    exit 1
fi

# 编译游戏
# 注意-lSDL2_image必须在最后
gcc -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE -L/opt/homebrew/lib \
    scripts/*.c -o mario_game -lSDL2 -lSDL2_image

# 检查编译结果
if [ $? -eq 0 ]; then
    echo "编译成功！"
    echo "运行游戏: ./mario_game"
    ls -lh mario_game
else
    echo "编译失败！"
    exit 1
fi 
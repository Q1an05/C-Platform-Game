# 超级玛丽游戏 Makefile

# 编译器设置
CC = gcc
CFLAGS = -std=c99 -Wall
LDFLAGS = $(shell sdl2-config --cflags --libs) -lSDL2_image

# 源文件
SOURCES = main.c mario.c map.c render.c input.c camera.c blocks.c enemy.c sprite.c
HEADERS = mario.h map.h render.h input.h camera.h blocks.h enemy.h sprite.h

# 目标文件
TARGET = mario_game

# 素材文件夹
ASSETS_DIR = assets/sprites

# 默认目标
all: $(TARGET)

# 编译游戏
$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SOURCES)

# 创建素材文件夹
assets:
	mkdir -p $(ASSETS_DIR)
	@echo "素材文件夹已创建：$(ASSETS_DIR)"
	@echo "请将PNG素材文件放入此文件夹，详见 SPRITES_GUIDE.md"

# 运行游戏
run: $(TARGET)
	./$(TARGET)

# 清理编译文件
clean:
	rm -f $(TARGET)
	rm -rf *.dSYM

# 安装依赖（macOS）
install-deps:
	@echo "安装SDL2依赖..."
	brew install sdl2 sdl2_image

# 显示帮助
help:
	@echo "超级玛丽游戏 Makefile 使用说明："
	@echo ""
	@echo "make           - 编译游戏"
	@echo "make run       - 编译并运行游戏"
	@echo "make assets    - 创建素材文件夹"
	@echo "make clean     - 清理编译文件"
	@echo "make install-deps - 安装SDL2依赖（macOS）"
	@echo "make help      - 显示此帮助信息"
	@echo ""
	@echo "素材使用："
	@echo "1. make assets          # 创建素材文件夹"
	@echo "2. 添加PNG文件到 assets/sprites/"
	@echo "3. make run             # 运行游戏"

# 声明伪目标
.PHONY: all run clean assets install-deps help 
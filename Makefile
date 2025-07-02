# 超级骑士游戏 跨平台Makefile

# 检测操作系统
ifeq ($(OS),Windows_NT)
    # Windows系统
    PLATFORM = windows
    TARGET = knight_game.exe
    RM = del /Q
    MKDIR = mkdir
    PATH_SEP = \\
    EXT = .exe
    CLEAN_EXTRA = *.o *.obj
else
    # Unix-like系统 (macOS, Linux)
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        # macOS
        PLATFORM = macos
        TARGET = knight_game
        RM = rm -f
        MKDIR = mkdir -p
        PATH_SEP = /
        EXT =
        CLEAN_EXTRA = *.dSYM
    else
        # Linux
        PLATFORM = linux
        TARGET = knight_game
        RM = rm -f
        MKDIR = mkdir -p
        PATH_SEP = /
        EXT =
        CLEAN_EXTRA = *.so
    endif
endif

# 编译器设置
CC = gcc
CFLAGS = -std=c99 -Wall

# 根据平台设置链接参数
ifeq ($(PLATFORM),windows)
    # Windows (MSYS2/MinGW)
    LDFLAGS = $(shell pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer)
else ifeq ($(PLATFORM),macos)
    # macOS
    LDFLAGS = $(shell sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer
else
    # Linux
    LDFLAGS = $(shell pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer)
endif

# 源文件（使用平台无关的路径分隔符）
SCRIPT_DIR = scripts
SOURCES = $(SCRIPT_DIR)/main.c $(SCRIPT_DIR)/knight.c $(SCRIPT_DIR)/map.c $(SCRIPT_DIR)/render.c $(SCRIPT_DIR)/input.c $(SCRIPT_DIR)/camera.c $(SCRIPT_DIR)/blocks.c $(SCRIPT_DIR)/enemy.c $(SCRIPT_DIR)/ui.c $(SCRIPT_DIR)/sound.c
HEADERS = $(SCRIPT_DIR)/knight.h $(SCRIPT_DIR)/map.h $(SCRIPT_DIR)/render.h $(SCRIPT_DIR)/input.h $(SCRIPT_DIR)/camera.h $(SCRIPT_DIR)/blocks.h $(SCRIPT_DIR)/enemy.h $(SCRIPT_DIR)/ui.h $(SCRIPT_DIR)/sound.h

# 素材文件夹
ASSETS_DIR = assets$(PATH_SEP)sprites

# 默认目标
all: $(TARGET)

# 编译游戏
$(TARGET): $(SOURCES) $(HEADERS)
	@echo "编译平台: $(PLATFORM)"
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SOURCES)

# 创建素材文件夹
assets:
	$(MKDIR) $(ASSETS_DIR)
	@echo "素材文件夹已创建：$(ASSETS_DIR)"
	@echo "请将PNG素材文件放入此文件夹"

# 运行游戏
run: $(TARGET)
ifeq ($(PLATFORM),windows)
	@echo "正在检查Windows PATH配置..."
	@where SDL2.dll >nul 2>&1 || (echo "警告: SDL2.dll未在PATH中找到，可能需要运行setup_path.bat" && echo "提示: 如果游戏无法启动，请运行 setup_path.bat 配置环境")
	$(TARGET)
else
	./$(TARGET)
endif

# 清理编译文件
clean:
	$(RM) $(TARGET)
ifeq ($(PLATFORM),windows)
	if exist *.o $(RM) *.o
	if exist *.obj $(RM) *.obj
else ifeq ($(PLATFORM),macos)
	$(RM) *.dSYM
	$(RM) -rf knight_game.dSYM
else
	$(RM) *.o
endif

# 平台特定的依赖安装指令
install-deps:
ifeq ($(PLATFORM),windows)
	@echo "Windows SDL2依赖安装指南："
	@echo ""
	@echo "方式1 - MSYS2 + PATH配置 (推荐):"
	@echo "1. 安装MSYS2: https://www.msys2.org/"
	@echo "2. 在MSYS2 MINGW64终端运行:"
	@echo "   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer"
	@echo "3. 配置PATH环境变量:"
	@echo "   setup_path.bat"
	@echo ""
	@echo "方式2 - vcpkg (高级用户):"
	@echo "   vcpkg install sdl2 sdl2-image sdl2-ttf sdl2-mixer"
	@echo ""
else ifeq ($(PLATFORM),macos)
	@echo "安装SDL2依赖..."
	brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer
else
	@echo "Linux SDL2依赖安装："
	@echo "Ubuntu/Debian: sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev"
	@echo "Fedora: sudo dnf install SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel"
	@echo "Arch: sudo pacman -S sdl2 sdl2_image sdl2_ttf sdl2_mixer"
endif

# 显示帮助
help:
	@echo "超级骑士游戏跨平台Makefile使用说明"
	@echo "当前平台: $(PLATFORM)"
	@echo ""
	@echo "make           - 编译游戏"
	@echo "make run       - 编译并运行游戏"
	@echo "make assets    - 创建素材文件夹"
	@echo "make clean     - 清理编译文件"
	@echo "make install-deps - 显示依赖安装指南"
	@echo "make help      - 显示此帮助信息"
	@echo ""
	@echo "素材使用："
	@echo "1. make assets          # 创建素材文件夹"
	@echo "2. 添加PNG文件到 assets/sprites/"
	@echo "3. make run             # 运行游戏"
	@echo ""
	@echo "注意：首次编译前请先安装SDL2依赖库"

# 声明伪目标
.PHONY: all run clean assets install-deps help 
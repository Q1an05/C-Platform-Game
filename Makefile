# Super Knight Game Cross-platform Makefile

# Detect operating system
ifeq ($(OS),Windows_NT)
    # Windows system
    PLATFORM = windows
    TARGET = knight_game.exe
    RM = rm -f
    MKDIR = mkdir
    PATH_SEP = \\
    EXT = .exe
    CLEAN_EXTRA = *.o *.obj
else
    # Unix-like systems (macOS, Linux)
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

# Compiler settings
CC = gcc
CFLAGS = -std=c99 -Wall

# Platform-specific linker flags
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

# Source files (using platform-independent path separators)
SCRIPT_DIR = scripts
SOURCES = $(SCRIPT_DIR)/main.c $(SCRIPT_DIR)/knight.c $(SCRIPT_DIR)/map.c $(SCRIPT_DIR)/render.c $(SCRIPT_DIR)/input.c $(SCRIPT_DIR)/camera.c $(SCRIPT_DIR)/blocks.c $(SCRIPT_DIR)/enemy.c $(SCRIPT_DIR)/ui.c $(SCRIPT_DIR)/sound.c
HEADERS = $(SCRIPT_DIR)/knight.h $(SCRIPT_DIR)/map.h $(SCRIPT_DIR)/render.h $(SCRIPT_DIR)/input.h $(SCRIPT_DIR)/camera.h $(SCRIPT_DIR)/blocks.h $(SCRIPT_DIR)/enemy.h $(SCRIPT_DIR)/ui.h $(SCRIPT_DIR)/sound.h

# Assets folder
ASSETS_DIR = assets$(PATH_SEP)sprites

# Default target
all: $(TARGET)

# Compile game
$(TARGET): $(SOURCES) $(HEADERS)
	@echo "Building for platform: $(PLATFORM)"
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

# Create assets folder
assets:
	$(MKDIR) $(ASSETS_DIR)
	@echo "Assets folder created: $(ASSETS_DIR)"
	@echo "Please place PNG sprite files in this folder"

# Run game
run: $(TARGET)
ifeq ($(PLATFORM),windows)
	./$(TARGET)
else
	./$(TARGET)
endif

# Clean build files
clean:
	$(RM) $(TARGET)
ifeq ($(PLATFORM),windows)
	$(RM) *.o *.obj
else ifeq ($(PLATFORM),macos)
	$(RM) *.dSYM
	$(RM) -rf knight_game.dSYM
else
	$(RM) *.o
endif

# Platform-specific dependency installation guide
install-deps:
ifeq ($(PLATFORM),windows)
	@echo "Windows SDL2 dependency installation guide:"
	@echo ""
	@echo "1. Install MSYS2: https://www.msys2.org/"
	@echo "2. Add C:\\msys64\\mingw64\\bin to system PATH environment variable"
	@echo "3. Run in MSYS2 MINGW64 terminal:"
	@echo "   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer"
	@echo ""
else ifeq ($(PLATFORM),macos)
	@echo "Installing SDL2 dependencies..."
	brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer
else
	@echo "Linux SDL2 dependency installation:"
	@echo "Ubuntu/Debian: sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev"
	@echo "Fedora: sudo dnf install SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel"
	@echo "Arch: sudo pacman -S sdl2 sdl2_image sdl2_ttf sdl2_mixer"
endif

# Show help
help:
	@echo "Super Knight Game Cross-platform Makefile Usage"
	@echo "Current platform: $(PLATFORM)"
	@echo ""
	@echo "make           - Compile game"
	@echo "make run       - Compile and run game"
	@echo "make assets    - Create assets folder"
	@echo "make clean     - Clean build files"
	@echo "make install-deps - Show dependency installation guide"
	@echo "make help      - Show this help information"
	@echo ""
	@echo "Asset usage:"
	@echo "1. make assets          # Create assets folder"
	@echo "2. Add PNG files to assets/sprites/"
	@echo "3. make run             # Run game"
	@echo ""
	@echo "Note: Please install SDL2 dependencies before first compilation"

# Declare phony targets
.PHONY: all run clean assets install-deps help 
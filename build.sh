#!/bin/bash
# 超级骑士游戏编译脚本

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_info() {
    echo -e "${BLUE}[信息]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[成功]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[警告]${NC} $1"
}

print_error() {
    echo -e "${RED}[错误]${NC} $1"
}

# 检查依赖
check_dependencies() {
    print_info "检查依赖..."
    
    # 检查SDL2
    if ! command -v sdl2-config &> /dev/null; then
        print_error "SDL2未安装"
        print_info "请运行: brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer"
        exit 1
    fi
    
    # 检查pkg-config（某些系统需要）
    if command -v pkg-config &> /dev/null; then
        # 检查SDL2_image
        if ! pkg-config --exists SDL2_image; then
            print_warning "SDL2_image可能未正确安装"
        fi
        
        # 检查SDL2_ttf
        if ! pkg-config --exists SDL2_ttf; then
            print_warning "SDL2_ttf可能未正确安装"
        fi
        
        # 检查SDL2_mixer
        if ! pkg-config --exists SDL2_mixer; then
            print_warning "SDL2_mixer可能未正确安装"
        fi
    fi
    
    print_success "依赖检查完成"
}

# 清理编译文件
clean() {
    print_info "清理编译文件..."
    rm -f knight_game
    rm -rf knight_game.dSYM
    print_success "清理完成"
}

# 编译游戏
compile() {
    print_info "编译骑士游戏..."
    
    # 编译参数
    CC="gcc"
    CFLAGS="-std=c99 -Wall"
    LDFLAGS="$(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer"
    SOURCES="scripts/main.c scripts/knight.c scripts/map.c scripts/render.c scripts/input.c scripts/camera.c scripts/blocks.c scripts/enemy.c scripts/ui.c scripts/sound.c"
    TARGET="knight_game"
    
    # 显示编译命令
    print_info "编译命令: ${CC} ${CFLAGS} ${LDFLAGS} -o ${TARGET} ${SOURCES}"
    
    # 执行编译
    ${CC} ${CFLAGS} ${LDFLAGS} -o ${TARGET} ${SOURCES}
    
    # 检查编译结果
    if [ $? -eq 0 ]; then
        print_success "编译成功！"
        if [ -f "${TARGET}" ]; then
            ls -lh "${TARGET}"
            print_info "运行游戏: ./${TARGET}"
        fi
        return 0
    else
        print_error "编译失败！"
        return 1
    fi
}

# 运行游戏
run() {
    if [ ! -f "knight_game" ]; then
        print_warning "游戏未编译，正在编译..."
        compile
        if [ $? -ne 0 ]; then
            exit 1
        fi
    fi
    
    print_info "启动游戏..."
    ./knight_game
}

# 安装依赖
install_deps() {
    print_info "安装SDL2依赖..."
    if command -v brew &> /dev/null; then
        brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer
        print_success "依赖安装完成"
    else
        print_error "未找到Homebrew，请手动安装SDL2相关库"
        print_info "或者安装Homebrew: /bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
        exit 1
    fi
}

# 显示帮助
show_help() {
    echo -e "${BLUE}超级骑士游戏编译脚本${NC}"
    echo ""
    echo "用法: $0 [选项]"
    echo ""
    echo "选项:"
    echo "  build, compile    编译游戏"
    echo "  run              编译并运行游戏"
    echo "  clean            清理编译文件"
    echo "  deps, install    安装依赖"
    echo "  help, -h         显示帮助信息"
    echo ""
    echo "示例:"
    echo "  $0                # 默认编译游戏"
    echo "  $0 run           # 编译并运行"
    echo "  $0 clean         # 清理文件"
    echo ""
}

# 主逻辑
case "${1:-build}" in
    "build"|"compile"|"")
        check_dependencies
        compile
        ;;
    "run")
        check_dependencies
        run
        ;;
    "clean")
        clean
        ;;
    "deps"|"install")
        install_deps
        ;;
    "help"|"-h"|"--help")
        show_help
        ;;
    *)
        print_error "未知选项: $1"
        show_help
        exit 1
        ;;
esac 
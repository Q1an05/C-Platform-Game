# C语言课程设计作业/超级骑士游戏

基于C语言和SDL2开发的2D平台跳跃游戏，实现了完整的物理系统、动画渲染、音效系统和敌人AI。

## 游戏特性

### 核心功能
- 角色移动（左右键）和跳跃（空格键）
- 重力物理系统和碰撞检测
- 卷轴式摄像机跟随
- 特殊方块系统
- 敌人系统
- UI系统（支持中英文切换）
- 道具（技能）系统

### 视觉系统
- **混合渲染模式**：地图使用PNG纹理，角色使用动画纹理，其他元素使用彩色矩形
- **智能纹理选择**：系统自动根据方块位置选择草地或泥土纹理
- **动画播放系统**：角色动画以固定0.1秒/帧的速度播放
- **平滑渲染**：优化浮点数计算，消除移动卡顿
- **无敌闪烁**：受伤时角色会闪烁显示

### 游戏元素
- 草地方块：使用grass.png纹理（16x16像素）
- 泥土方块：使用mud.png纹理（16x16像素）
- 角色：使用动画纹理系统（15x20像素）
  - 静止：4帧idle动画循环播放
  - 移动：16帧run动画流畅播放
  - 自动水平翻转，真实方向感
- 敌人：16x16像素动画
- 天蓝色背景

## 操作说明

- **左/右方向键**：移动
- **空格键**：跳跃
- **D键**：冲刺（获得技能后）
- **ESC键**：退出游戏

## 地图编辑

使用字符在map.c的二维数组中定义地图：

```
'G' = 草地（地表，使用grass.png）
'M' = 泥土（地下，使用mud.png）
'#' = 普通砖块（可选，棕色矩形）
' ' = 空地（透明）
'F' = 二连跳技能方块
'D' = 冲刺技能方块
'E' = 敌人位置（游戏启动时自动转换为敌人）
'B' = 敌人的不可见障碍，用于设定敌人活动范围
'C' = 摄像机移动不可见方块，角色触碰时会将摄像机镜头右移
'S' = 平台跳跃前的存档点，角色掉下去会回到存档点
'T' = 陷阱方块，不可见，角色触碰会回到存档点
't' = 通关方块（绿色终点，触碰即通关）
```

## 系统要求

- **操作系统**: macOS / Windows / Linux（跨平台支持）
- **编译器**: GCC (推荐) 或 Clang
- **依赖库**: SDL2, SDL2_image, SDL2_ttf, SDL2_mixer

### 各平台具体要求

**macOS**:
- 包管理器: Homebrew
- 编译器: GCC（系统自带或Xcode Command Line Tools）

**Windows**:
- 推荐环境: MSYS2 + MinGW-w64
- 编译器: GCC (MinGW) 或 MSVC

**Linux**:
- 包管理器: apt, dnf, pacman等
- 编译器: GCC（系统自带）

## 快速开始

### macOS / Linux
```bash
# 1. 进入项目目录
cd 小学期作业

# 2. 安装依赖（首次运行）
./build.sh deps

# 3. 编译并运行游戏
./build.sh run
```

### Windows
```cmd
# 1. 安装MSYS2和相关依赖
# 2. 在MSYS2 MINGW64终端运行
make run
```

### 跨平台（使用Makefile）
```bash
make install-deps    # 查看依赖安装指南
make run            # 编译并运行
```

## 详细安装步骤

### 步骤1：环境准备

**macOS用户：**
```bash
# 检查是否已安装Homebrew
brew --version

# 如果没有安装，运行以下命令
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

**Windows用户：**
1. 下载并安装MSYS2: https://www.msys2.org/
2. 打开MSYS2 MINGW64终端
3. 更新包数据库: `pacman -Syu`
4. 重启终端，再次更新: `pacman -Su`
5. 将 `C:\msys64\mingw64\bin` 添加到系统PATH环境变量

**Linux用户：**
```bash
# Ubuntu/Debian - 更新包管理器
sudo apt update

# CentOS/RHEL/Fedora - 更新包管理器  
sudo dnf update  # 或 sudo yum update

# Arch Linux - 更新包管理器
sudo pacman -Syu
```

### 步骤2：安装依赖库

**macOS：**
```bash
# 方式1：使用项目脚本（推荐）
./build.sh deps

# 方式2：直接使用Homebrew
brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer

# 方式3：使用Makefile
make install-deps
```

**Windows（MSYS2）：**
```bash
# 在MSYS2 MINGW64终端中运行
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
```

**Linux：**
```bash
# Ubuntu/Debian
sudo apt-get install make pkg-config libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev

# Fedora/CentOS
sudo dnf install make pkgconf SDL2-devel SDL2_image-devel SDL2_ttf-devel SDL2_mixer-devel

# Arch Linux
sudo pacman -S make sdl2 sdl2_image sdl2_ttf sdl2_mixer
```

### 步骤3：编译游戏

**macOS / Linux：**
```bash
# 方式1：使用shell脚本（推荐）
./build.sh              # 仅编译
./build.sh run          # 编译并运行游戏

# 方式2：使用Makefile
make                    # 编译
make run               # 编译并运行

# 方式3：手动编译（macOS）
gcc -std=c99 -Wall $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_mixer -o knight_game scripts/*.c

# 方式3：手动编译（Linux）
gcc -std=c99 -Wall $(pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer) -o knight_game scripts/*.c
```

**Windows：**
```cmd
# 推荐在MSYS2 MINGW64终端中使用make
make                   # 编译
make run              # 编译并运行

# 或者手动编译
gcc -std=c99 -Wall $(pkg-config --cflags --libs sdl2 SDL2_image SDL2_ttf SDL2_mixer) -o knight_game.exe scripts/*.c
```

### 步骤4：运行游戏

**macOS / Linux：**
```bash
./knight_game
```

**Windows：**
```cmd
# 在MSYS2 MINGW64终端中运行
./knight_game.exe
```

## 构建脚本功能

### build.sh (macOS/Linux)

```bash
./build.sh                # 默认编译游戏
./build.sh build         # 编译游戏
./build.sh run           # 编译并运行游戏
./build.sh clean         # 清理编译文件
./build.sh deps          # 安装依赖库
./build.sh help          # 显示帮助信息
```

### Makefile (跨平台)

```bash
make                     # 编译游戏
make run                # 编译并运行游戏
make clean              # 清理编译文件
make install-deps       # 显示依赖安装指南
make help               # 显示帮助信息
```

## 常见问题解决

### 问题1：SDL2依赖库未找到

**macOS错误**：`SDL2未安装` 或 `command not found: sdl2-config`
**解决方案**：
```bash
./build.sh deps
# 或
brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer
```

**Windows错误**：`GCC编译器未找到` 或 `SDL2.dll缺失`
**解决方案**：
1. 确保已安装MSYS2
2. 确保已将 `C:\msys64\mingw64\bin` 添加到系统PATH
3. 重新打开MSYS2 MINGW64终端
4. 安装依赖包

**Linux错误**：`pkg-config: command not found` 或依赖库未找到
**解决方案**：
```bash
# Ubuntu/Debian
sudo apt-get install pkg-config libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev

# 给build.sh执行权限
chmod +x build.sh
```

### 问题2：包管理器未安装

**macOS**：`command not found: brew`
**解决方案**：
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

**Windows**：`MSYS2未安装` 或 `pacman: command not found`
**解决方案**：
1. 下载并安装MSYS2: https://www.msys2.org/
2. 将MSYS2的mingw64/bin目录添加到系统PATH环境变量
3. 重启命令提示符或MSYS2终端

### 问题3：音频文件缺失

确保`assets/sounds/`目录包含所有音频文件：
- `jump.wav` - 跳跃音效
- `hurt.wav` - 受伤音效
- `power_up.wav` - 获得技能音效
- `coin.wav` - 通关音效
- `explosion.wav` - 击杀敌人音效
- `tap.wav` - 菜单切换音效
- `music.mp3` - 背景音乐

### 问题4：图片素材缺失

确保`assets/sprites/`目录包含所有图片文件：
```
assets/sprites/
├── player/          # 角色动画
├── enemy/           # 敌人动画
├── rewards/         # 奖励道具
└── world/           # 地形纹理
```

## 性能和特性

### 编译输出
- **可执行文件大小**：约60KB
- **编译时间**：通常1-3秒
- **运行内存占用**：约10-20MB

### 游戏性能
- **帧率**：60 FPS
- **分辨率**：320x176 (16:9比例)
- **格子大小**：16x16像素
- **视野范围**：20x11格子

### 技术特性
- **完整音效系统**：7种音效 + 背景音乐循环播放
- **动画系统**：角色多状态动画，支持水平翻转
- **智能地图系统**：支持重置，数据驱动的敌人生成
- **UI系统**：游戏提示、菜单系统、多语言支持
- **输入系统**：响应式按键处理，支持连发和单次触发

## 项目结构

```
小学期作业/
├── scripts/               # 所有源代码文件
│   ├── main.c             # 主程序和游戏循环
│   ├── knight.c/h         # 角色逻辑和物理系统
│   ├── enemy.c/h          # 敌人AI和碰撞系统
│   ├── map.c/h            # 地图数据和地形管理
│   ├── render.c/h         # SDL2渲染和纹理管理
│   ├── camera.c/h         # 摄像机跟随系统
│   ├── blocks.c/h         # 奖励方块系统
│   ├── input.c/h          # 输入处理和按键映射
│   ├── ui.c/h             # 用户界面和提示系统
│   └── sound.c/h          # 音效系统和音频管理
├── assets/                # 游戏资源文件
│   ├── fonts/            # 字体文件
│   ├── sounds/           # 音频文件
│   └── sprites/          # 图片素材
├── build.sh              # 智能编译脚本
├── Makefile              # Make配置文件
└── README.md             # 项目说明文档
```

## 技术特点

### 核心技术栈
- **编程语言**：标准C语言（C99）
- **图形库**：SDL2 + SDL2_image + SDL2_ttf + SDL2_mixer
- **编译器**：GCC with C99标准
- **构建系统**：Make + 自定义Shell脚本

### 架构设计
- **模块化设计**：每个功能独立成模块，便于维护和扩展
- **数据驱动**：地图、敌人位置、音效配置均由数据文件控制
- **状态管理**：完整的游戏状态机，支持菜单、游戏、暂停等状态
- **资源管理**：智能的纹理和音频资源加载与释放

### 性能优化
- **硬件加速渲染**：使用SDL2硬件加速纹理渲染
- **智能碰撞检测**：优化的AABB碰撞算法
- **内存管理**：及时释放资源，避免内存泄漏
- **帧率控制**：固定60FPS，保证游戏流畅运行

## 开发学习收获

- 掌握了SDL2图形编程基础和高级特性
- 实现了完整的2D游戏物理系统
- 学会了模块化程序设计和大型项目组织
- 理解了游戏循环、状态管理和事件处理
- 实践了碰撞检测、动画系统和音效管理
- 体验了从功能实现到性能优化的完整开发流程
- 学会了构建系统设计和跨平台兼容性处理
- 掌握了资源管理、内存优化和错误处理
- 理解了游戏开发中的数学计算和算法应用

## 素材引用

- 贴图、音乐素材：https://brackeysgames.itch.io/brackeys-platformer-bundle
- 字体素材：https://github.com/scott0107000/BoutiqueBitmap9x9

---

**快速开始命令：**

```bash
# macOS/Linux
./build.sh run

# Windows (在MSYS2 MINGW64终端中)
make run
```

**完整Windows部署步骤：**
1. 安装MSYS2: https://www.msys2.org/
2. 将 `C:\msys64\mingw64\bin` 添加到系统PATH环境变量
3. 在MSYS2 MINGW64终端安装依赖: `pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer`
4. 编译运行: `make run`

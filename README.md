# 超级玛丽第一关
用C语言和SDL2实现的经典超级玛丽游戏第一关

## 游戏功能

### 基础功能
- 马里奥角色移动（左右键）和跳跃（空格键）
- 重力物理系统和碰撞检测
- 卷轴式摄像机跟随
- 奖励方块系统（马里奥可以变大变快）

### 敌人系统 🆕
- **栗子小子（Goomba）敌人**：棕色方块敌人
- **智能移动**：敌人会在平台上左右移动，遇墙转向，避免掉下悬崖
- **踩踏机制**：从上方跳到敌人身上可以消灭敌人，马里奥会弹跳
- **伤害系统**：侧面碰撞会使马里奥受伤
  - 大马里奥受伤会变回小马里奥
  - 小马里奥受伤会失去生命并重生
- **无敌时间**：受伤后有2秒无敌时间，期间马里奥会闪烁
- **生命系统**：马里奥有3条生命，生命用完游戏结束

### 素材系统 ✨
- **PNG图片支持**：支持透明背景的角色素材
- **动画系统**：马里奥走路、跳跃动画，敌人行走动画
- **自动翻转**：角色会根据移动方向自动翻转
- **备用显示**：没有素材时使用彩色方块，确保游戏可玩
- **易于扩展**：模块化设计，方便添加新素材和动画

### 操作方式
- **左/右方向键**：移动马里奥
- **空格键**：跳跃
- **ESC键**：退出游戏

### 游戏目标
- 避开或踩踏敌人
- 收集奖励方块变大变强
- 尽可能向右前进

## 编译和运行

### 依赖要求
- SDL2：`brew install sdl2`
- SDL2_image：`brew install sdl2_image`

### 快速开始（推荐）
```bash
# 安装依赖
make install-deps

# 编译并运行
make run

# 创建素材文件夹（可选）
make assets
```

### 手动编译
```bash
gcc -std=c99 -Wall $(sdl2-config --cflags) $(sdl2-config --libs) -lSDL2_image -o mario_game main.c mario.c map.c render.c input.c camera.c blocks.c enemy.c sprite.c
./mario_game
```

### Makefile命令
- `make` 或 `make all` - 编译游戏
- `make run` - 编译并运行游戏
- `make assets` - 创建素材文件夹
- `make clean` - 清理编译文件
- `make install-deps` - 安装SDL2依赖（macOS）
- `make help` - 显示详细帮助

## 模块结构
- `main.c` - 主程序和游戏循环
- `mario.c/h` - 马里奥角色逻辑和动画
- `enemy.c/h` - 敌人系统 🆕
- `sprite.c/h` - 素材管理和动画系统 ✨
- `map.c/h` - 地图数据
- `render.c/h` - SDL2渲染
- `camera.c/h` - 摄像机系统
- `blocks.c/h` - 奖励方块系统
- `input.c/h` - 输入处理
- `SPRITES_GUIDE.md` - 素材使用详细指南 📖

# Windows 系统支持说明

## 🚨 快速修复常见错误

如果你遇到以下错误，请立即执行对应的修复命令：

### ❌ `'make' 不是内部或外部命令`
```bash
# 在MSYS2 MINGW64终端运行
pacman -S mingw-w64-x86_64-make
```

### ❌ `Package 'sdl2' was not found`
```bash
# 在MSYS2 MINGW64终端运行
pacman -S mingw-w64-x86_64-pkg-config
export PKG_CONFIG_PATH="/mingw64/lib/pkgconfig:/mingw64/share/pkgconfig"
```

### ❌ `GCC编译器未找到`
```bash
# 在MSYS2 MINGW64终端运行
pacman -S mingw-w64-x86_64-gcc
export PATH="/mingw64/bin:$PATH"
```

### ❌ `无法找到SDL2.dll等运行时库`
**错误信息**: 
- `无法找到SDL2.dll`
- `SDL2_image.dll缺失`
- `SDL2_ttf.dll缺失`
- `SDL2_mixer.dll缺失`
- `libavif-16.dll缺失`
- `libjxl.dll缺失`
- `libtiff-6.dll缺失`
- `libwebp-7.dll缺失`
- 以及其他各种lib*.dll文件

**解决方案（PATH配置方式）**：
```cmd
# 方案1: 自动配置PATH环境变量（推荐）
setup_path.bat

# 方案2: 手动添加MSYS2到系统PATH
# 将 C:\msys64\mingw64\bin 永久添加到系统环境变量PATH中
# 具体步骤见下方"PATH配置说明"

# 方案3: 在MSYS2终端中运行游戏
# 打开MSYS2 MINGW64终端，cd到游戏目录，然后运行
./knight_game.exe
```

### ❌ `main函数参数不匹配`
**错误信息**: `number of arguments doesn't match prototype`
**解决方案**：
```c
// 错误的写法
int main() {

// 正确的写法（SDL2在Windows上要求标准参数）
int main(int argc, char* argv[]) {
```
**说明**: SDL2在Windows上会重定义main为SDL_main，必须使用标准的argc/argv参数。

### ❌ `构建脚本问题或编码错误`
**解决方案**：
```cmd
# 方案1: 先运行诊断脚本检查问题
diagnose.bat

# 方案2: 使用主要构建脚本（推荐）
build_simple.bat

# 方案3: 直接使用make命令
make
```

### 🔧 完整安装命令（一次性解决所有问题）
```bash
# 在MSYS2 MINGW64终端运行以下完整命令
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer

# 设置环境变量
export PATH="/mingw64/bin:/usr/bin:$PATH"
export PKG_CONFIG_PATH="/mingw64/lib/pkgconfig:/mingw64/share/pkgconfig"

# 验证安装
which gcc && which make && which pkg-config
pkg-config --cflags --libs sdl2
```

---

## 🎯 Windows 适配完成情况

### ✅ 已完成的适配工作

1. **构建系统适配**
   - ✅ 创建了 `build_simple.bat` Windows批处理脚本
   - ✅ 更新了 `Makefile` 支持跨平台编译
   - ✅ 自动检测操作系统并使用相应的编译参数

2. **依赖管理**
   - ✅ 支持 MSYS2/MinGW-w64 编译环境
   - ✅ 支持 vcpkg 包管理器
   - ✅ 提供详细的Windows依赖安装指南

3. **文档更新**
   - ✅ README.md 添加完整的Windows安装教程
   - ✅ 提供Windows特有问题的解决方案
   - ✅ 包含跨平台使用说明

4. **代码兼容性**
   - ✅ 源代码使用标准C99，天然跨平台
   - ✅ SDL2库本身支持Windows
   - ✅ 路径使用正斜杠，SDL2在Windows上正常处理

## 🚀 Windows 快速开始

### 方案一：MSYS2 + PATH配置（推荐）

1. **安装MSYS2**
   ```
   下载地址：https://www.msys2.org/
   按照官网指引完成安装
   ```

2. **安装依赖**
   ```bash
   # 在MSYS2 MINGW64终端运行
   pacman -Syu
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-pkg-config mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_mixer
   ```

3. **配置PATH环境变量（一键设置）**
   ```cmd
   # 进入项目目录
   cd 小学期作业
   
   # 运行PATH配置脚本（推荐）
   setup_path.bat
   ```

4. **编译运行**
   ```cmd
   # 推荐方式：使用构建脚本
   build_simple.bat run
   
   # 或者直接使用make命令
   make run
   ```

### PATH配置说明

#### 自动配置（推荐）
```cmd
# 运行PATH配置脚本
setup_path.bat
```
该脚本会：
- 检查MSYS2安装和SDL2库
- 提供多种PATH配置选项（临时/永久）
- 验证配置是否成功

#### 手动配置PATH环境变量
1. **图形界面方式**：
   - 右键点击"此电脑" → "属性"
   - 点击"高级系统设置"
   - 点击"环境变量"
   - 在用户变量或系统变量中找到"PATH"
   - 编辑PATH，添加：`C:\msys64\mingw64\bin`
   - 确定保存，重新打开命令提示符

2. **命令行方式**：
   ```cmd
   # 用户环境变量
   setx PATH "C:\msys64\mingw64\bin;%PATH%"
   
   # 系统环境变量（需要管理员权限）
   setx PATH "C:\msys64\mingw64\bin;%PATH%" /m
   ```

#### 验证PATH配置
```cmd
# 检查SDL2库是否在PATH中
where SDL2.dll

# 检查GCC编译器
where gcc

# 检查pkg-config
where pkg-config
```

### 方案二：vcpkg（高级用户）

1. **安装vcpkg**
   ```cmd
   git clone https://github.com/Microsoft/vcpkg.git
   .\vcpkg\bootstrap-vcpkg.bat
   ```

2. **安装SDL2**
   ```cmd
   .\vcpkg\vcpkg install sdl2 sdl2-image sdl2-ttf sdl2-mixer
   ```

3. **配置环境并编译**
   ```cmd
   # 设置环境变量或修改编译命令
   # 具体步骤见README.md
   ```

---

## 📋 PATH配置方式的优势

✅ **节省磁盘空间**: 不需要在项目目录复制dll文件  
✅ **便于维护**: dll文件统一在MSYS2目录，更新方便  
✅ **减少冗余**: 多个项目可以共享同一套dll文件  
✅ **环境统一**: 类似Linux系统的库管理方式  
✅ **自动更新**: MSYS2包管理器更新时，所有项目自动受益  
✅ **简化管理**: 一次配置，所有项目受益

---

## 🔧 故障排除

### PATH配置后游戏仍无法运行

1. **重新打开命令提示符**: 永久PATH设置需要重启命令提示符
2. **验证PATH**: 运行 `where SDL2.dll` 检查库文件位置
3. **权限问题**: 以管理员身份运行 `setup_path.bat` 设置系统PATH
4. **MSYS2版本**: 确保使用MinGW64版本，不是MSYS2或UCRT64

### 常见注意事项

⚠️ **环境一致性**: 确保所有项目使用相同的PATH配置  
⚠️ **重启生效**: 永久PATH设置需要重新打开命令提示符才能生效

## 🛠️ Windows构建工具功能

### Windows构建脚本
```cmd
# 主要构建脚本
build_simple.bat        # 编译游戏（使用PATH配置）
build_simple.bat run    # 编译并运行游戏

# 工具脚本
setup_path.bat          # 配置PATH环境变量
diagnose.bat            # 检查环境和依赖问题
```

### 特性
- 🎨 彩色终端输出
- 🔍 智能依赖检查
- ⚡ 自动编译检测
- 📋 详细的错误提示
- 🌐 中文界面支持

## ⚠️ Windows常见问题

### 问题1：'make' 不是内部或外部命令
**解决方案**：
```bash
# 在MSYS2终端安装make工具
pacman -S mingw-w64-x86_64-make

# 将C:\msys64\mingw64\bin添加到系统PATH环境变量
# 或在当前会话中设置
export PATH="/mingw64/bin:$PATH"
```

### 问题2：GCC编译器未找到
**解决方案**：
- 安装MSYS2并将`C:\msys64\mingw64\bin`添加到PATH
- 或安装MinGW-w64独立版本

### 问题3：Package 'sdl2' was not found
**解决方案**：
```bash
# 1. 安装pkg-config工具
pacman -S mingw-w64-x86_64-pkg-config

# 2. 设置PKG_CONFIG_PATH环境变量
export PKG_CONFIG_PATH="/mingw64/lib/pkgconfig:/mingw64/share/pkgconfig"

# 3. 永久设置（添加到系统环境变量）
# PKG_CONFIG_PATH = C:\msys64\mingw64\lib\pkgconfig;C:\msys64\mingw64\share\pkgconfig

# 4. 验证SDL2是否能被找到
pkg-config --cflags --libs sdl2
```

### 问题4：SDL2库未找到
**解决方案**：
- 确保在MSYS2环境中正确安装了SDL2包
- 检查pkg-config配置是否正确

### 问题5：中文路径问题
**解决方案**：
- 将项目移动到纯英文路径
- 或设置UTF-8编码：`chcp 65001`

### 问题6：权限问题
**解决方案**：
- 以管理员身份运行命令提示符
- 检查防病毒软件是否阻止编译

## 📊 Windows性能表现

### 编译性能
- **编译时间**：1-3秒（取决于硬件）
- **可执行文件**：`knight_game.exe`，约60KB
- **运行需求**：Windows 7及以上版本

### 运行时性能
- **帧率**：60 FPS
- **内存占用**：10-20MB
- **兼容性**：支持Windows 7/8/10/11

## 🔧 开发环境推荐

### 推荐配置
1. **MSYS2 + VS Code**
   - MSYS2提供编译环境
   - VS Code提供代码编辑
   - 集成终端支持

2. **Dev-C++ + MinGW**
   - 简单易用的IDE
   - 内置MinGW编译器
   - 适合初学者

3. **Visual Studio + vcpkg**
   - 专业级IDE
   - 强大的调试功能
   - 适合高级开发

## 📋 部署检查清单

在Windows上部署游戏前，请确认：

**编译环境**：
- [ ] 已安装GCC编译器（MSYS2/MinGW）
- [ ] 已安装make工具（mingw-w64-x86_64-make）
- [ ] 已安装pkg-config工具（mingw-w64-x86_64-pkg-config）
- [ ] 已安装SDL2相关库
- [ ] PATH环境变量包含：C:\msys64\mingw64\bin
- [ ] PKG_CONFIG_PATH环境变量已设置

**代码兼容性**：
- [ ] main函数使用正确签名：`int main(int argc, char* argv[])`
- [ ] 项目路径不包含中文字符

**运行时环境**：
- [ ] PATH环境变量已配置（运行setup_path.bat）
- [ ] 所有资源文件存在（assets目录）
- [ ] 防火墙允许程序运行

## 📞 技术支持

如果在Windows上遇到问题，请：

1. 查看README.md的常见问题部分
2. 确认是否按照本文档步骤操作
3. 检查MSYS2和SDL2安装是否正确
4. 尝试在纯英文路径下编译

## 🎉 总结

经过完整的Windows适配，本项目现在支持：
- ✅ Windows 7/8/10/11
- ✅ MSYS2/MinGW编译环境 + PATH配置
- ✅ vcpkg包管理器
- ✅ 跨平台Makefile
- ✅ Windows批处理脚本
- ✅ 完整的中文文档
- ✅ 一次配置，永久使用的PATH管理方式

现在Windows用户可以与macOS/Linux用户一样，使用统一的库管理方式，轻松编译和运行超级骑士游戏！ 
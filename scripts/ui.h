// ui.h
// UI和菜单系统头文件

#ifndef UI_H
#define UI_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

// 游戏状态枚举
typedef enum {
    GAME_STATE_MAIN_MENU,    // 主菜单
    GAME_STATE_PLAYING,      // 游戏进行中
    GAME_STATE_PAUSED,       // 游戏暂停
    GAME_STATE_GAME_OVER     // 游戏结束
} GameState;

// 菜单选项枚举
typedef enum {
    MENU_OPTION_START,       // 开始游戏
    MENU_OPTION_CONTINUE,    // 继续游戏（暂停菜单）
    MENU_OPTION_RESTART,     // 重新开始
    MENU_OPTION_QUIT,        // 退出游戏
    MENU_OPTION_COUNT        // 选项总数
} MenuOption;

// UI系统初始化和清理
int init_ui();              // 初始化UI系统（加载字体等）
void cleanup_ui();          // 清理UI资源

// 游戏状态管理
GameState get_game_state();
void set_game_state(GameState state);

// 菜单相关函数
void update_menu(SDL_Event* e);           // 更新菜单输入
void render_main_menu();                   // 渲染主菜单
void render_pause_menu();                  // 渲染暂停菜单
void render_game_over_screen();            // 渲染游戏结束画面
int get_selected_menu_option();            // 获取当前选中的菜单选项
void reset_menu_selection();               // 重置菜单选择

// 游戏内UI渲染
void render_game_ui();                     // 渲染游戏内UI（生命值、分数等）
void render_text(const char* text, int x, int y, SDL_Color color, int center); // 渲染文本

// 语言设置
typedef enum {
    LANG_CHINESE,       // 中文
    LANG_ENGLISH        // 英文
} Language;

// 语言管理
Language get_current_language();           // 获取当前语言
void set_language(Language lang);          // 设置语言
void toggle_language();                    // 切换语言
const char* get_text(const char* key);     // 根据key获取当前语言的文本

// UI效果
void show_damage_indicator();              // 显示受伤效果
void update_ui_effects(float delta_time);  // 更新UI效果

// 游戏提示系统
void show_game_start_hint();               // 显示游戏开始提示
void show_skill_hint(const char* skill_name); // 显示技能获得提示
void render_game_hints();                  // 渲染游戏提示

#endif // UI_H
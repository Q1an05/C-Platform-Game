// ui.c
// UI和菜单系统实现

#include "ui.h"
#include "render.h"
#include "knight.h"
#include "sound.h"
#include <stdio.h>
#include <string.h>

// 全局变量
static TTF_Font* game_font = NULL;       // 游戏字体
static GameState current_game_state = GAME_STATE_MAIN_MENU;
static int selected_menu_option = 0;     // 当前选中的菜单选项
static float damage_indicator_timer = 0.0f;  // 受伤效果计时器
static Language current_language = LANG_CHINESE;  // 当前语言，默认中文

// 游戏提示系统变量
static float game_start_hint_timer = 0.0f;    // 游戏开始提示计时器
static float skill_hint_timer = 0.0f;         // 技能提示计时器
static char skill_hint_text[128] = "";        // 技能提示文本

// 窗口尺寸（与render.c保持一致）
#define WINDOW_WIDTH 320   // 逻辑宽度
#define WINDOW_HEIGHT 176  // 逻辑高度

// UI颜色定义
static SDL_Color color_white = {255, 255, 255, 255};
static SDL_Color color_yellow = {255, 255, 0, 255};
static SDL_Color color_red = {255, 0, 0, 255};
static SDL_Color color_gray = {128, 128, 128, 255};

// 文本对照表结构
typedef struct {
    const char* key;
    const char* chinese;
    const char* english;
} TextEntry;

// 文本对照表
static const TextEntry text_table[] = {
    // 主菜单
    {"title", "超级骑士", "Super Knight"},
    {"start_game", "开始游戏", "Start Game"},
    {"quit_game", "退出游戏", "Quit Game"},
    {"menu_help", "↑↓选择 Enter确认 Tab切换语言", "UP/DOWN select, Enter confirm, Tab language"},
    
    // 暂停菜单
    {"game_paused", "游戏暂停", "Game Paused"},
    {"continue", "继续游戏", "Continue"},
    {"restart", "重新开始", "Restart"},
    {"pause_help", "ESC返回游戏 Tab切换语言", "ESC resume, Tab language"},
    
    // 游戏结束
    {"game_over", "游戏结束", "Game Over"},
    {"game_win", "恭喜通关", "Congratulations!"},
    {"you_win", "你成功到达了终点！", "You have reached the goal!"},
    
    // 游戏内UI
    {"lives", "生命", "Lives"},
    {"invincible", "无敌中", "Invincible"},
    {"pause_hint", "ESC-暂停", "ESC-Pause"},
    
    // 游戏提示
    {"start_hint", "方向键左右移动，空格键跳跃", "Arrow keys to move, Space to jump"},
    {"double_jump_hint", "已获得二连跳技能", "Double Jump skill acquired"},
    {"dash_hint", "已获得冲刺技能，按D键冲刺", "Dash skill acquired, press D to dash"},
    
    // 语言切换
    {"lang_switched_cn", "切换为中文", "Switched to Chinese"},
    {"lang_switched_en", "切换为英文", "Switched to English"},
    
    {NULL, NULL, NULL} // 表结束标记
};

// 初始化UI系统
int init_ui() {
    // 初始化SDL_ttf
    if (TTF_Init() == -1) {
        printf("SDL_ttf初始化失败: %s\n", TTF_GetError());
        return 0;
    }
    
    // 加载字体
    game_font = TTF_OpenFont("assets/fonts/BoutiqueBitmap9x9_1.9.ttf", 13); // 使用9号字体大小（适合像素风格）
    if (!game_font) {
        printf("字体加载失败: %s\n", TTF_GetError());
        return 0;
    }
    
    printf("UI系统初始化成功\n");
    return 1;
}

// 清理UI资源
void cleanup_ui() {
    if (game_font) {
        TTF_CloseFont(game_font);
        game_font = NULL;
    }
    TTF_Quit();
}

// 游戏状态管理
GameState get_game_state() {
    return current_game_state;
}

void set_game_state(GameState state) {
    current_game_state = state;
    // 切换到任何菜单时重置选择
    if (state == GAME_STATE_MAIN_MENU || state == GAME_STATE_PAUSED || state == GAME_STATE_GAME_OVER) {
        reset_menu_selection();
    }
}

// 获取当前选中的菜单选项
int get_selected_menu_option() {
    return selected_menu_option;
}

// 重置菜单选择
void reset_menu_selection() {
    selected_menu_option = 0;
}

// 更新菜单输入
void update_menu(SDL_Event* e) {
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP:
            case SDLK_w:
                selected_menu_option--;
                play_sound(SOUND_TAP); // 播放菜单切换音效
                if (selected_menu_option < 0) {
                    // 根据当前菜单设置合适的最大选项
                    if (current_game_state == GAME_STATE_MAIN_MENU) {
                        selected_menu_option = 1; // 开始和退出
                    } else if (current_game_state == GAME_STATE_PAUSED) {
                        selected_menu_option = 2; // 继续、重新开始和退出
                    } else if (current_game_state == GAME_STATE_GAME_OVER) {
                        selected_menu_option = 1; // 重新开始和退出
                    }
                }
                break;
                
            case SDLK_DOWN:
            case SDLK_s:
                selected_menu_option++;
                play_sound(SOUND_TAP); // 播放菜单切换音效
                if (current_game_state == GAME_STATE_MAIN_MENU && selected_menu_option > 1) {
                    selected_menu_option = 0;
                } else if (current_game_state == GAME_STATE_PAUSED && selected_menu_option > 2) {
                    selected_menu_option = 0;
                } else if (current_game_state == GAME_STATE_GAME_OVER && selected_menu_option > 1) {
                    selected_menu_option = 0;
                }
                break;
                
            case SDLK_TAB:
                // Tab键切换语言
                toggle_language();
                break;
                
            case SDLK_RETURN:
            case SDLK_SPACE:
                // 在主函数中处理选中的选项
                break;
        }
    }
}

// 渲染文本
void render_text(const char* text, int x, int y, SDL_Color color, int center) {
    if (!game_font || !gRenderer) return;
    
    SDL_Surface* text_surface = NULL;
    
    // 根据当前语言选择合适的渲染方式
    if (current_language == LANG_CHINESE) {
        // 中文使用UTF8渲染
        text_surface = TTF_RenderUTF8_Solid(game_font, text, color);
    } else {
        // 英文使用ASCII渲染（更兼容）
        text_surface = TTF_RenderText_Solid(game_font, text, color);
    }
    
    if (!text_surface) {
        printf("文本渲染失败: %s, 错误: %s\n", text, TTF_GetError());
        return;
    }
    
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(gRenderer, text_surface);
    if (!text_texture) {
        SDL_FreeSurface(text_surface);
        return;
    }
    
    SDL_Rect dest_rect;
    dest_rect.w = text_surface->w;
    dest_rect.h = text_surface->h;
    
    if (center) {
        dest_rect.x = x - dest_rect.w / 2;
        dest_rect.y = y - dest_rect.h / 2;
    } else {
        dest_rect.x = x;
        dest_rect.y = y;
    }
    
    SDL_RenderCopy(gRenderer, text_texture, NULL, &dest_rect);
    
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}

// 渲染主菜单
void render_main_menu() {
    // 渲染标题
    render_text(get_text("title"), WINDOW_WIDTH/2, 40, color_white, 1);
    
    // 渲染菜单选项
    SDL_Color start_color = (selected_menu_option == 0) ? color_yellow : color_white;
    SDL_Color quit_color = (selected_menu_option == 1) ? color_yellow : color_white;
    
    render_text(get_text("start_game"), WINDOW_WIDTH/2, 80, start_color, 1);
    render_text(get_text("quit_game"), WINDOW_WIDTH/2, 100, quit_color, 1);
    
    // 渲染提示
    render_text(get_text("menu_help"), WINDOW_WIDTH/2, 140, color_gray, 1);
}

// 渲染暂停菜单
void render_pause_menu() {
    // 半透明背景
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 128);
    SDL_Rect screen_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(gRenderer, &screen_rect);
    
    // 渲染标题
    render_text(get_text("game_paused"), WINDOW_WIDTH/2, 40, color_white, 1);
    
    // 渲染菜单选项
    SDL_Color continue_color = (selected_menu_option == 0) ? color_yellow : color_white;
    SDL_Color restart_color = (selected_menu_option == 1) ? color_yellow : color_white;
    SDL_Color quit_color = (selected_menu_option == 2) ? color_yellow : color_white;
    
    render_text(get_text("continue"), WINDOW_WIDTH/2, 70, continue_color, 1);
    render_text(get_text("restart"), WINDOW_WIDTH/2, 90, restart_color, 1);
    render_text(get_text("quit_game"), WINDOW_WIDTH/2, 110, quit_color, 1);
    
    // 渲染提示
    render_text(get_text("pause_help"), WINDOW_WIDTH/2, 140, color_gray, 1);
}

// 渲染游戏结束画面
void render_game_over_screen() {
    // 半透明背景
    SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 192);
    SDL_Rect screen_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(gRenderer, &screen_rect);
    
    // 检查是通关还是死亡（通过骑士生命值判断）
    int knight_lives = knight_get_lives();
    
    if (knight_lives > 0) {
        // 通关情况
        render_text(get_text("game_win"), WINDOW_WIDTH/2, 40, color_yellow, 1);
        render_text(get_text("you_win"), WINDOW_WIDTH/2, 60, color_white, 1);
    } else {
        // 死亡情况
        render_text(get_text("game_over"), WINDOW_WIDTH/2, 60, color_red, 1);
    }
    
    // 渲染选项
    SDL_Color restart_color = (selected_menu_option == 0) ? color_yellow : color_white;
    SDL_Color quit_color = (selected_menu_option == 1) ? color_yellow : color_white;
    
    render_text(get_text("restart"), WINDOW_WIDTH/2, 90, restart_color, 1);
    render_text(get_text("quit_game"), WINDOW_WIDTH/2, 110, quit_color, 1);
}

// 渲染游戏内UI
void render_game_ui() {
    // 获取骑士生命值
    int lives = knight_get_lives();
    
    // 渲染生命值
    char lives_text[32];
    const char* lives_label = get_text("lives");
    snprintf(lives_text, sizeof(lives_text), "%s: %d", lives_label, lives);
    render_text(lives_text, 10, 10, color_white, 0);
    
    // 渲染生命值图标（根据语言选择不同符号）
    const char* heart_symbol = (current_language == LANG_CHINESE) ? "♥" : "*";
    for (int i = 0; i < lives; i++) {
        render_text(heart_symbol, 70 + i * 12, 10, color_red, 0);
    }
    
    // 如果骑士处于无敌状态，显示闪烁效果
    if (knight_is_invulnerable()) {
        // 使用SDL_GetTicks获取时间来创建闪烁效果
        if ((SDL_GetTicks() / 100) % 2 == 0) {
            render_text(get_text("invincible"), 10, 25, color_yellow, 0);
        }
    }
    
    // 渲染受伤效果
    if (damage_indicator_timer > 0) {
        // 红色半透明覆盖层
        SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
        Uint8 alpha = (Uint8)(damage_indicator_timer / 0.5f * 64); // 最大64透明度
        SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, alpha);
        SDL_Rect screen_rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        SDL_RenderFillRect(gRenderer, &screen_rect);
    }
    
    // 渲染控制提示（右上角）
    render_text(get_text("pause_hint"), WINDOW_WIDTH - 70, 10, color_gray, 0);
}

// 显示受伤效果
void show_damage_indicator() {
    damage_indicator_timer = 0.5f; // 0.5秒的受伤效果
}

// 更新UI效果
void update_ui_effects(float delta_time) {
    // 更新受伤效果计时器
    if (damage_indicator_timer > 0) {
        damage_indicator_timer -= delta_time;
        if (damage_indicator_timer < 0) {
            damage_indicator_timer = 0;
        }
    }
    
    // 更新游戏开始提示计时器
    if (game_start_hint_timer > 0) {
        game_start_hint_timer -= delta_time;
        if (game_start_hint_timer < 0) {
            game_start_hint_timer = 0;
        }
    }
    
    // 更新技能提示计时器
    if (skill_hint_timer > 0) {
        skill_hint_timer -= delta_time;
        if (skill_hint_timer < 0) {
            skill_hint_timer = 0;
        }
    }
}

// 语言管理函数
Language get_current_language() {
    return current_language;
}

void set_language(Language lang) {
    current_language = lang;
    printf("语言已切换为: %s\n", (lang == LANG_CHINESE) ? "中文" : "English");
}

void toggle_language() {
    current_language = (current_language == LANG_CHINESE) ? LANG_ENGLISH : LANG_CHINESE;
    printf("语言已切换为: %s\n", (current_language == LANG_CHINESE) ? "中文" : "English");
}

// 根据key获取当前语言的文本
const char* get_text(const char* key) {
    for (int i = 0; text_table[i].key != NULL; i++) {
        if (strcmp(text_table[i].key, key) == 0) {
            if (current_language == LANG_CHINESE) {
                return text_table[i].chinese;
            } else {
                return text_table[i].english;
            }
        }
    }
    // 如果找不到对应的key，返回key本身作为备用
    return key;
}

// 游戏提示系统实现

// 显示游戏开始提示
void show_game_start_hint() {
    game_start_hint_timer = 3.0f; // 显示3秒
}

// 显示技能获得提示
void show_skill_hint(const char* skill_name) {
    if (strcmp(skill_name, "double_jump") == 0) {
        strncpy(skill_hint_text, get_text("double_jump_hint"), sizeof(skill_hint_text) - 1);
    } else if (strcmp(skill_name, "dash") == 0) {
        strncpy(skill_hint_text, get_text("dash_hint"), sizeof(skill_hint_text) - 1);
    } else {
        strncpy(skill_hint_text, skill_name, sizeof(skill_hint_text) - 1);
    }
    skill_hint_text[sizeof(skill_hint_text) - 1] = '\0'; // 确保字符串结束
    skill_hint_timer = 2.5f; // 显示2.5秒
}

// 渲染游戏提示
void render_game_hints() {
    // 渲染游戏开始提示
    if (game_start_hint_timer > 0) {
        // 计算透明度（淡入淡出效果）
        float alpha_factor = 1.0f;
        if (game_start_hint_timer > 2.5f) {
            // 前0.5秒淡入
            alpha_factor = (3.0f - game_start_hint_timer) / 0.5f;
        } else if (game_start_hint_timer < 0.5f) {
            // 后0.5秒淡出
            alpha_factor = game_start_hint_timer / 0.5f;
        }
        
        // 创建半透明背景
        SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
        Uint8 bg_alpha = (Uint8)(alpha_factor * 128);
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, bg_alpha);
        SDL_Rect bg_rect = {WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT - 40, 200, 25};
        SDL_RenderFillRect(gRenderer, &bg_rect);
        
        // 渲染提示文本
        SDL_Color hint_color = {255, 255, 255, (Uint8)(alpha_factor * 255)};
        render_text(get_text("start_hint"), WINDOW_WIDTH/2, WINDOW_HEIGHT - 27, hint_color, 1);
    }
    
    // 渲染技能获得提示
    if (skill_hint_timer > 0) {
        // 计算透明度（淡入淡出效果）
        float alpha_factor = 1.0f;
        if (skill_hint_timer > 2.0f) {
            // 前0.5秒淡入
            alpha_factor = (2.5f - skill_hint_timer) / 0.5f;
        } else if (skill_hint_timer < 0.5f) {
            // 后0.5秒淡出
            alpha_factor = skill_hint_timer / 0.5f;
        }
        
        // 创建半透明背景
        SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
        Uint8 bg_alpha = (Uint8)(alpha_factor * 150);
        SDL_SetRenderDrawColor(gRenderer, 0, 128, 0, bg_alpha); // 绿色背景表示获得技能
        SDL_Rect bg_rect = {WINDOW_WIDTH/2 - 120, WINDOW_HEIGHT/2 - 15, 240, 30};
        SDL_RenderFillRect(gRenderer, &bg_rect);
        
        // 渲染技能提示文本
        SDL_Color skill_color = {255, 255, 0, (Uint8)(alpha_factor * 255)}; // 黄色文字
        render_text(skill_hint_text, WINDOW_WIDTH/2, WINDOW_HEIGHT/2, skill_color, 1);
    }
}
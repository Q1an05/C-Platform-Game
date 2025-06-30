// render.c
// SDL2渲染模块实现，负责显示地图和马里奥

#include <SDL.h>
#include <math.h>
#include "map.h"
#include "mario.h"
#include "camera.h"
#include "blocks.h"
#include "enemy.h"
#include "render.h"

// 全局窗口和渲染器指针
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

// 每个格子的像素大小
#define TILE_SIZE 16

// 颜色定义
SDL_Color COLOR_BG       = {135, 206, 235, 255}; // 天空蓝
SDL_Color COLOR_WALL     = {139, 69, 19, 255};   // 棕色（砖块）
SDL_Color COLOR_MARIO    = {255, 0, 0, 255};     // 红色（马里奥）
SDL_Color COLOR_REWARD   = {255, 255, 0, 255};   // 黄色（奖励方块）
SDL_Color COLOR_USED     = {160, 160, 160, 255}; // 灰色（用过的方块）
SDL_Color COLOR_ENEMY    = {101, 67, 33, 255};   // 棕色（敌人）
SDL_Color COLOR_ENEMY_DEAD = {80, 80, 80, 255};  // 深灰色（死亡敌人）

// 绘制彩色矩形的辅助函数
void draw_colored_rect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color) {
    // 保存当前绘制颜色
    Uint8 old_r, old_g, old_b, old_a;
    SDL_GetRenderDrawColor(renderer, &old_r, &old_g, &old_b, &old_a);
    
    // 设置新颜色
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    // 绘制填充矩形
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
    
    // 恢复原来的颜色
    SDL_SetRenderDrawColor(renderer, old_r, old_g, old_b, old_a);
}

// 初始化SDL2窗口和渲染器
int init_render() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 0;
    }
    
    // 调整窗口大小适应新的TILE_SIZE
    int window_width = 32 * TILE_SIZE;  // 32个格子宽度
    int window_height = MAP_HEIGHT * TILE_SIZE;
    
    gWindow = SDL_CreateWindow("超级玛丽", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height, SDL_WINDOW_SHOWN);
    if (!gWindow) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        return 0;
    }
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!gRenderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        return 0;
    }
    
    // 初始化摄像机
    init_camera(window_width, window_height);
    
    printf("渲染系统初始化成功！格子大小: %dx%d\n", TILE_SIZE, TILE_SIZE);
    return 1;
}

// 渲染游戏画面
void render_game() {
    // 清屏（天空蓝）
    SDL_SetRenderDrawColor(gRenderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
    SDL_RenderClear(gRenderer);

    // 获取摄像机浮点数位置
    float camera_x_float, camera_y_float;
    get_camera_offset(&camera_x_float, &camera_y_float);
    
    // 使用浮点数偏移量，只在最终渲染位置时转换为整数
    float render_offset_x = camera_x_float;
    float render_offset_y = camera_y_float;

    // 绘制地图（只渲染视野内的部分）
    int start_x = (int)(camera_x_float / TILE_SIZE);
    int end_x = start_x + (32 * TILE_SIZE) / TILE_SIZE + 2;
    if (start_x < 0) start_x = 0;
    if (end_x > MAP_WIDTH) end_x = MAP_WIDTH;
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = start_x; x < end_x; x++) {
            // 计算屏幕坐标，使用整数坐标避免子像素渲染
            SDL_Rect rect = {
                (int)(x * TILE_SIZE - render_offset_x), 
                (int)(y * TILE_SIZE - render_offset_y), 
                TILE_SIZE, 
                TILE_SIZE
            };
            
            // 检查方块类型并绘制
            BlockType block_type = get_block_type(x, y);
            
            if (block_type == BLOCK_REWARD) {
                // 奖励方块：检查是否被撞击过
                if (is_block_hit(x, y)) {
                    draw_colored_rect(gRenderer, rect.x, rect.y, rect.w, rect.h, COLOR_USED);
                } else {
                    draw_colored_rect(gRenderer, rect.x, rect.y, rect.w, rect.h, COLOR_REWARD);
                }
            } else if (block_type == BLOCK_NORMAL || game_map[y][x] == '#') {
                // 普通砖块
                draw_colored_rect(gRenderer, rect.x, rect.y, rect.w, rect.h, COLOR_WALL);
            }
        }
    }

    // 绘制敌人
    for (int i = 0; i < enemy_count; i++) {
        float enemy_x, enemy_y;
        int enemy_w, enemy_h;
        EnemyState enemy_state;
        
        get_enemy_info(i, &enemy_x, &enemy_y, &enemy_w, &enemy_h, &enemy_state);
        
        // 只渲染存活的或正在死亡动画的敌人
        if (enemy_state == ENEMY_STATE_DEAD) continue;
        
        // 计算屏幕坐标，使用平滑的浮点数计算
        SDL_Rect enemyRect = {
            (int)(enemy_x - render_offset_x),
            (int)(enemy_y - render_offset_y),
            enemy_w,
            enemy_h
        };
        
        // 根据敌人状态绘制
        if (enemy_state == ENEMY_STATE_STOMPED) {
            draw_colored_rect(gRenderer, enemyRect.x, enemyRect.y, enemyRect.w, enemyRect.h, COLOR_ENEMY_DEAD);
        } else {
            draw_colored_rect(gRenderer, enemyRect.x, enemyRect.y, enemyRect.w, enemyRect.h, COLOR_ENEMY);
        }
    }

    // 绘制马里奥（最后绘制，确保在前景）
    float mario_world_x, mario_world_y;
    int mario_w, mario_h;
    get_mario_position(&mario_world_x, &mario_world_y);
    get_mario_size(&mario_w, &mario_h);
    
    // 计算屏幕坐标，使用平滑的浮点数计算
    SDL_Rect marioRect = {
        (int)(mario_world_x - render_offset_x), 
        (int)(mario_world_y - render_offset_y), 
        mario_w, 
        mario_h
    };
    
    // 如果马里奥无敌，实现闪烁效果
    int should_draw = 1;
    if (mario_is_invulnerable()) {
        // 使用简单的时间计算实现闪烁
        static int flash_counter = 0;
        flash_counter++;
        should_draw = (flash_counter / 6) % 2; // 每6帧切换一次
    }
    
    if (should_draw) {
        draw_colored_rect(gRenderer, marioRect.x, marioRect.y, marioRect.w, marioRect.h, COLOR_MARIO);
    }

    SDL_RenderPresent(gRenderer);
}

// 释放SDL2资源
void cleanup_render() {
    if (gRenderer) SDL_DestroyRenderer(gRenderer);
    if (gWindow) SDL_DestroyWindow(gWindow);
    SDL_Quit();
    printf("渲染系统已清理！\n");
} 
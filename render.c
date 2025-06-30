// render.c
// SDL2渲染模块实现，负责显示地图和马里奥

#include <SDL.h>
#include "map.h"
#include "mario.h"
#include "camera.h"
#include "blocks.h"
#include "enemy.h"
#include "sprite.h"
#include "render.h"

// 全局窗口和渲染器指针
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

// 每个格子的像素大小
#define TILE_SIZE 32

// 颜色定义
SDL_Color COLOR_BG       = {135, 206, 235, 255}; // 天空蓝
SDL_Color COLOR_WALL     = {139, 69, 19, 255};   // 棕色（砖块）
SDL_Color COLOR_MARIO    = {255, 0, 0, 255};     // 红色（马里奥）
SDL_Color COLOR_REWARD   = {255, 255, 0, 255};   // 黄色（奖励方块）
SDL_Color COLOR_USED     = {160, 160, 160, 255}; // 灰色（用过的方块）
SDL_Color COLOR_ENEMY    = {101, 67, 33, 255};   // 棕色（敌人）
SDL_Color COLOR_ENEMY_DEAD = {80, 80, 80, 255};  // 深灰色（死亡敌人）

// 初始化SDL2窗口和渲染器
int init_render() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 0;
    }
    
    // 窗口大小保持不变，但地图比窗口大
    int window_width = 16 * TILE_SIZE;  // 保持原来的窗口大小
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
    
    // 初始化素材系统
    if (!init_sprite_system(gRenderer)) {
        SDL_Log("素材系统初始化失败！");
        return 0;
    }
    
    // 加载所有素材
    load_all_sprites(gRenderer);
    
    // 初始化摄像机
    init_camera(window_width, window_height);
    
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
    
    // 为本帧渲染创建一个稳定的、四舍五入的整数偏移量
    // 这是解决"抖动"和"闪回"的关键
    int render_offset_x = roundf(camera_x_float);
    int render_offset_y = roundf(camera_y_float);

    // 绘制地图（只渲染视野内的部分）
    // 确定要渲染哪些图块时，仍然使用浮点数位置，以确保图块平滑地进入和离开屏幕
    int start_x = (int)(camera_x_float / TILE_SIZE);
    int end_x = start_x + (16 * TILE_SIZE) / TILE_SIZE + 2;
    if (start_x < 0) start_x = 0;
    if (end_x > MAP_WIDTH) end_x = MAP_WIDTH;
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = start_x; x < end_x; x++) {
            // 计算屏幕坐标时使用稳定的整数偏移量
            SDL_Rect rect = {
                x * TILE_SIZE - render_offset_x, 
                y * TILE_SIZE - render_offset_y, 
                TILE_SIZE, 
                TILE_SIZE
            };
            
            // 检查方块类型并绘制对应素材
            BlockType block_type = get_block_type(x, y);
            
            if (block_type == BLOCK_REWARD) {
                // 奖励方块：检查是否被撞击过
                if (is_block_hit(x, y)) {
                    draw_sprite_scaled(gRenderer, SPRITE_BLOCK_USED, rect.x, rect.y, rect.w, rect.h);
                } else {
                    draw_sprite_scaled(gRenderer, SPRITE_BLOCK_QUESTION, rect.x, rect.y, rect.w, rect.h);
                }
            } else if (block_type == BLOCK_NORMAL || game_map[y][x] == '#') {
                // 普通砖块
                draw_sprite_scaled(gRenderer, SPRITE_BLOCK_BRICK, rect.x, rect.y, rect.w, rect.h);
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
        
        // 计算屏幕坐标
        SDL_Rect enemyRect = {
            (int)roundf(enemy_x) - render_offset_x,
            (int)roundf(enemy_y) - render_offset_y,
            enemy_w,
            enemy_h
        };
        
        // 根据敌人状态绘制对应素材
        if (enemy_state == ENEMY_STATE_STOMPED) {
            draw_sprite_scaled(gRenderer, SPRITE_GOOMBA_DEAD, enemyRect.x, enemyRect.y, enemyRect.w, enemyRect.h);
        } else {
            // 简单的走路动画（每30帧切换一次）
            static int anim_counter = 0;
            anim_counter++;
            SpriteID goomba_sprite = ((anim_counter / 30) % 2) ? SPRITE_GOOMBA_WALK1 : SPRITE_GOOMBA_WALK2;
            draw_sprite_scaled(gRenderer, goomba_sprite, enemyRect.x, enemyRect.y, enemyRect.w, enemyRect.h);
        }
    }

    // 绘制马里奥（最后绘制，确保在前景）
    float mario_world_x, mario_world_y;
    int mario_w, mario_h;
    get_mario_position(&mario_world_x, &mario_world_y);
    get_mario_size(&mario_w, &mario_h);
    
    // 计算屏幕坐标时，将世界坐标四舍五入，并减去稳定的整数偏移量
    SDL_Rect marioRect = {
        (int)roundf(mario_world_x) - render_offset_x, 
        (int)roundf(mario_world_y) - render_offset_y, 
        mario_w, 
        mario_h
    };
    
    // 如果马里奥无敌，实现闪烁效果
    int should_draw = 1;
    if (mario_is_invulnerable()) {
        // 使用简单的时间计算实现闪烁（每0.1秒切换一次可见性）
        static int flash_counter = 0;
        flash_counter++;
        should_draw = (flash_counter / 6) % 2; // 每6帧切换一次（约0.1秒）
    }
    
    if (should_draw) {
        // 根据马里奥状态和动画选择精灵
        SpriteID mario_sprite = get_mario_sprite();
        
        // 根据面向方向决定是否翻转
        SDL_RendererFlip flip = mario.facing_right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        
        // 使用精灵绘制
        if (flip == SDL_FLIP_NONE) {
            draw_sprite_scaled(gRenderer, mario_sprite, marioRect.x, marioRect.y, marioRect.w, marioRect.h);
        } else {
            // 需要单独处理翻转情况
            if (is_sprite_loaded(mario_sprite)) {
                SDL_RenderCopyEx(gRenderer, sprites[mario_sprite].texture, NULL, &marioRect, 0.0, NULL, flip);
            } else {
                // 备用绘制
                draw_fallback_sprite(gRenderer, mario_sprite, marioRect.x, marioRect.y, marioRect.w, marioRect.h);
            }
        }
    }

    SDL_RenderPresent(gRenderer);
}

// 释放SDL2资源
void cleanup_render() {
    cleanup_sprite_system();
    if (gRenderer) SDL_DestroyRenderer(gRenderer);
    if (gWindow) SDL_DestroyWindow(gWindow);
    SDL_Quit();
} 
// render.c
// SDL2渲染模块实现，负责显示地图和马里奥

#include <SDL2/SDL.h>
#include "map.h"
#include "mario.h"
#include "camera.h"
#include "render.h"

// 全局窗口和渲染器指针
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

// 每个格子的像素大小
#define TILE_SIZE 32

// 颜色定义
SDL_Color COLOR_BG    = {135, 206, 235, 255}; // 天空蓝
SDL_Color COLOR_WALL  = {139, 69, 19, 255};   // 棕色（砖块）
SDL_Color COLOR_MARIO = {255, 0, 0, 255};     // 红色（马里奥）

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
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!gRenderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        return 0;
    }
    
    // 初始化摄像机
    init_camera(window_width, window_height);
    
    return 1;
}

// 渲染游戏画面
void render_game() {
    // 清屏（天空蓝）
    SDL_SetRenderDrawColor(gRenderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
    SDL_RenderClear(gRenderer);

    // 获取摄像机偏移
    float camera_x, camera_y;
    get_camera_offset(&camera_x, &camera_y);

    // 绘制地图（只渲染视野内的部分）
    int start_x = (int)(camera_x / TILE_SIZE);
    int end_x = start_x + (16 * TILE_SIZE) / TILE_SIZE + 2; // 多渲染2列防止边缘问题
    if (start_x < 0) start_x = 0;
    if (end_x > MAP_WIDTH) end_x = MAP_WIDTH;
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = start_x; x < end_x; x++) {
            char tile = game_map[y][x];
            if (tile == '#') {
                // 计算屏幕坐标
                float screen_x, screen_y;
                world_to_screen(x * TILE_SIZE, y * TILE_SIZE, &screen_x, &screen_y);
                
                SDL_Rect rect = {(int)screen_x, (int)screen_y, TILE_SIZE, TILE_SIZE};
                SDL_SetRenderDrawColor(gRenderer, COLOR_WALL.r, COLOR_WALL.g, COLOR_WALL.b, COLOR_WALL.a);
                SDL_RenderFillRect(gRenderer, &rect);
            }
        }
    }

    // 绘制马里奥（转换为屏幕坐标）
    float mario_world_x, mario_world_y;
    int mario_w, mario_h;
    get_mario_position(&mario_world_x, &mario_world_y);
    get_mario_size(&mario_w, &mario_h);
    
    float mario_screen_x, mario_screen_y;
    world_to_screen(mario_world_x, mario_world_y, &mario_screen_x, &mario_screen_y);
    
    SDL_Rect marioRect = {(int)mario_screen_x, (int)mario_screen_y, mario_w, mario_h};
    SDL_SetRenderDrawColor(gRenderer, COLOR_MARIO.r, COLOR_MARIO.g, COLOR_MARIO.b, COLOR_MARIO.a);
    SDL_RenderFillRect(gRenderer, &marioRect);

    SDL_RenderPresent(gRenderer);
}

// 释放SDL2资源
void cleanup_render() {
    if (gRenderer) SDL_DestroyRenderer(gRenderer);
    if (gWindow) SDL_DestroyWindow(gWindow);
    SDL_Quit();
} 
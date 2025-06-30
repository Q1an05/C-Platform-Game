// main.c
// 超级玛丽第一关 - 程序入口和主循环

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "map.h"
#include "mario.h"
#include "render.h"
#include "input.h"
#include "camera.h"

// 移动速度常量
#define MARIO_SPEED 4.0f

// 这里可以包含你自己写的头文件
// #include "map.h"
// #include "mario.h"
// #include "render.h"
// #include "input.h"

// 游戏是否结束的标志
bool game_over = false;

// 处理用户输入（基于input模块的状态）
void process_input() {
    // 处理移动输入
    if (is_action_pressed(INPUT_LEFT)) {
        set_mario_target_velocity(-MARIO_SPEED);
    } else if (is_action_pressed(INPUT_RIGHT)) {
        set_mario_target_velocity(MARIO_SPEED);
    } else {
        // 没有按左右键时，目标速度为0（摩擦力会逐渐减速）
        set_mario_target_velocity(0);
    }
    
    // 处理跳跃输入（只在刚按下时跳跃，避免连续跳跃）
    if (is_action_just_pressed(INPUT_JUMP)) {
        mario_jump();
    }
}

// 更新游戏状态
void update_game() {
    update_mario(); // 更新马里奥状态
}

int main() {
    // 初始化各个模块
    if (!init_render()) {
        printf("SDL2 初始化失败！\n");
        return 1;
    }
    
    init_mario();
    init_input();

    bool quit = false;
    SDL_Event e;

    // SDL2主循环
    while (!quit) {
        // 处理事件
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            
            // 更新输入状态
            update_input(&e);
        }
        
        // 检查退出输入
        if (is_action_just_pressed(INPUT_QUIT)) {
            quit = true;
        }

        process_input();
        update_game();
        
        // 更新摄像机（跟随马里奥）
        float mario_x, mario_y;
        get_mario_position(&mario_x, &mario_y);
        update_camera(mario_x, mario_y);
        
        // 更新输入帧状态（在帧结束时调用）
        update_input_frame();

        render_game();
        
        SDL_Delay(16); // 控制帧率，约60FPS
    }

    // 清理各个模块
    cleanup_input();
    cleanup_render();
    printf("游戏结束，感谢游玩！\n");
    return 0;
}


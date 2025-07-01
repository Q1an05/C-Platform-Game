// main.c
// 超级玛丽第一关 - 程序入口和主循环

#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "map.h"
#include "knight.h"
#include "render.h"
#include "input.h"
#include "camera.h"
#include "blocks.h"
#include "enemy.h"

// 移动速度常量
#define KNIGHT_SPEED 4.0f

// 这里可以包含你自己写的头文件
// #include "map.h"
// #include "knight.h"
// #include "render.h"
// #include "input.h"

// 游戏是否结束的标志
bool game_over = false;

// 处理用户输入（基于input模块的状态）
void process_input() {
    // 固定移动速度
    float speed = KNIGHT_SPEED;
    
    // 处理移动输入
    if (is_action_pressed(INPUT_LEFT)) {
        set_knight_target_velocity(-speed);
    } else if (is_action_pressed(INPUT_RIGHT)) {
        set_knight_target_velocity(speed);
    } else {
        // 没有按左右键时，目标速度为0（摩擦力会逐渐减速）
        set_knight_target_velocity(0);
    }
    
    // 处理跳跃输入（只在刚按下时跳跃，避免连续跳跃）
    if (is_action_just_pressed(INPUT_JUMP)) {
        knight_jump();
    }
}

// 更新游戏状态
void update_game() {
    update_knight(); // 更新骑士状态
    update_enemies(); // 更新敌人状态
    
    // 检查骑士与敌人的碰撞
    if (check_knight_enemy_collision()) {
        // 骑士受伤
        knight_take_damage();
    }
}

int main() {
    // 初始化各个模块
    if (!init_render()) {
        printf("SDL2 初始化失败！\n");
        return 1;
    }
    
    init_knight();
    init_input();
    init_blocks();
    init_enemies();

    bool quit = false;
    SDL_Event e;
    
    // 帧率控制变量
    const int TARGET_FPS = 60;
    const int FRAME_TIME = 1000 / TARGET_FPS; // 16.67ms
    
    // 累计时间变量，用于更精确的帧率控制
    Uint32 last_time = SDL_GetTicks();
    float time_accumulator = 0.0f;

    // SDL2主循环
    while (!quit) {
        Uint32 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f; // 转换为秒
        last_time = current_time;
        
        // 累计时间，确保稳定的更新频率
        time_accumulator += delta_time;
        const float fixed_timestep = 1.0f / TARGET_FPS;
        
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

        // 固定时间步长更新（确保游戏逻辑稳定）
        while (time_accumulator >= fixed_timestep) {
            process_input();
            update_game();
            update_blocks();  // 更新方块状态
            
            // 定期清理死亡的敌人（每秒一次）
            static int cleanup_counter = 0;
            cleanup_counter++;
            if (cleanup_counter >= TARGET_FPS) { // 每60帧（1秒）清理一次
                remove_dead_enemies();
                cleanup_counter = 0;
            }
            
            // 更新输入帧状态
            update_input_frame();
            
            time_accumulator -= fixed_timestep;
            
            if (game_over) {
                quit = true;
                printf("游戏通关，感谢游玩！\n");
                break;
            }
        }
        
        // 最后更新摄像机（在所有逻辑更新完成后）
        float knight_x, knight_y;
        get_knight_position(&knight_x, &knight_y);
        update_camera(knight_x, knight_y);

        render_game();
        
        // 确保不超过目标帧率
        Uint32 frame_end_time = SDL_GetTicks();
        Uint32 frame_duration = frame_end_time - current_time;
        if (frame_duration < FRAME_TIME) {
            SDL_Delay(FRAME_TIME - frame_duration);
        }
    }

    // 清理各个模块
    cleanup_input();
    cleanup_render();
    printf("游戏结束，感谢游玩！\n");
    return 0;
}


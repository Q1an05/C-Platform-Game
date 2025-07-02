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
#include "ui.h"
#include "sound.h"


// 游戏是否结束的标志
bool game_over = false;

// 更新游戏状态
void update_game() {
    update_knight(); // 更新骑士状态
    update_enemies(); // 更新敌人状态
    
    // 检查骑士与敌人的碰撞
    if (check_knight_enemy_collision()) {
        // 骑士受伤
        knight_take_damage();
        show_damage_indicator(); // 显示受伤效果
    }
    
    // 检查游戏结束条件
    if (knight_get_lives() <= 0) {
        set_game_state(GAME_STATE_GAME_OVER);
    }
}

// 重置游戏状态
void reset_game() {
    reset_map();       // 首先重置地图到初始状态
    init_knight();
    init_enemies();
    game_over = false;
    reset_menu_selection();
}

int main() {
    // 初始化各个模块
    if (!init_render()) {
        printf("SDL2 初始化失败！\n");
        return 1;
    }
    
    if (!init_ui()) {
        printf("UI系统初始化失败！\n");
        cleanup_render();
        return 1;
    }
    
    if (!init_sound_system()) {
        printf("音效系统初始化失败！\n");
        cleanup_ui();
        cleanup_render();
        return 1;
    }
    
    init_knight();
    init_input();
    init_enemies();
    
    // 开始播放背景音乐
    play_background_music();

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
            
            // 根据游戏状态处理输入
            GameState current_state = get_game_state();
            
            if (current_state == GAME_STATE_MAIN_MENU || 
                current_state == GAME_STATE_PAUSED ||
                current_state == GAME_STATE_GAME_OVER) {
                // 在菜单状态下处理菜单输入
                update_menu(&e);
                
                // ESC键在暂停菜单中返回游戏
                if (current_state == GAME_STATE_PAUSED && 
                    e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                    set_game_state(GAME_STATE_PLAYING);
                    resume_background_music(); // 恢复背景音乐
                }
                
                // 处理菜单选择确认
                if (e.type == SDL_KEYDOWN && 
                    (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE)) {
                    int option = get_selected_menu_option();
                    
                    if (current_state == GAME_STATE_MAIN_MENU) {
                        if (option == 0) { // 开始游戏
                            set_game_state(GAME_STATE_PLAYING);
                            reset_game();
                            show_game_start_hint(); // 显示游戏开始操作提示
                        } else if (option == 1) { // 退出
                            quit = true;
                        }
                    } else if (current_state == GAME_STATE_PAUSED) {
                        if (option == 0) { // 继续
                            set_game_state(GAME_STATE_PLAYING);
                            resume_background_music(); // 恢复背景音乐
                        } else if (option == 1) { // 重新开始
                            set_game_state(GAME_STATE_PLAYING);
                            reset_game();
                            show_game_start_hint(); // 重新开始时也显示提示
                            resume_background_music(); // 恢复背景音乐
                        } else if (option == 2) { // 退出
                            quit = true;
                        }
                    } else if (current_state == GAME_STATE_GAME_OVER) {
                        if (option == 0) { // 重新开始
                            set_game_state(GAME_STATE_PLAYING);
                            reset_game();
                            show_game_start_hint(); // 重新开始时也显示提示
                        } else if (option == 1) { // 退出
                            quit = true;
                        }
                    }
                }
            } else if (current_state == GAME_STATE_PLAYING) {
                // 游戏进行中，更新输入状态
                update_input(&e);
                
                // ESC键暂停游戏
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                    set_game_state(GAME_STATE_PAUSED);
                    pause_background_music(); // 暂停背景音乐
                }
                
                // Tab键切换语言（游戏中也可以切换）
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_TAB) {
                    toggle_language();
                }
            }
        }
        
        // 不再使用is_action_just_pressed检查ESC退出

        // 只在游戏进行中更新游戏逻辑
        if (get_game_state() == GAME_STATE_PLAYING) {
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
                
                time_accumulator -= fixed_timestep;
            }
            
            // 最后更新摄像机（在所有逻辑更新完成后）
            float knight_x, knight_y;
            get_knight_position(&knight_x, &knight_y);
            
            // 使用优化的摄像机更新函数，传递角色状态信息
            update_camera_with_state(knight_x, knight_y, knight.vx, knight.is_dashing, knight.facing_right);
        }
        
        // 更新UI效果（在所有游戏状态下都更新）
        update_ui_effects(delta_time);
        
        // 更新输入帧状态（在所有游戏状态下都更新，确保状态一致）
        update_input_frame();

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
    cleanup_sound_system();
    cleanup_ui();
    cleanup_render();
    printf("游戏结束，感谢游玩！\n");
    return 0;
}


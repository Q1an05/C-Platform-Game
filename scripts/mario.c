// mario.c
// 马里奥角色逻辑实现

#include "mario.h"
#include "map.h"
#include "blocks.h"
#include <stdio.h>
#include <math.h>

// 全局马里奥对象
Mario mario;

// 马里奥物理常量
#define MARIO_MAX_SPEED 2.0f
#define MARIO_ACCELERATION 0.8f
#define MARIO_WIDTH 15             // 宽度15像素
#define MARIO_HEIGHT 20            // 高度20像素

// 通用物理常量
#define GROUND_FRICTION 0.15f
#define AIR_FRICTION 0.05f
#define GRAVITY 0.5f
#define JUMP_FORCE -6.0f
#define MAX_FALL_SPEED 10.0f
#define TILE_SIZE 16

extern int game_over;

// 初始化马里奥
void init_mario() {
    mario.x = 2 * TILE_SIZE;      // 初始位置（像素坐标）
    mario.y = 6 * TILE_SIZE;
    mario.vx = 0.0f;              // 初始速度为0
    mario.vy = 0.0f;
    mario.target_vx = 0.0f;       // 初始目标速度为0
    mario.width = MARIO_WIDTH;    // 固定15x20尺寸
    mario.height = MARIO_HEIGHT;
    mario.alive = 1;              // 存活状态
    mario.on_ground = 0;          // 初始不在地面（会下落到地面）
    mario.lives = 3;              // 初始3条生命
    mario.hurt_timer = 0.0f;      // 初始无受伤状态
    mario.facing_right = 1;       // 初始面向右
    
    // 初始化动画状态
    mario.anim_state = MARIO_ANIM_IDLE;
    mario.anim_timer = 0.0f;
    mario.anim_frame = 0;
}

// 检查指定位置是否有碰撞（撞墙或超出边界）
int check_collision(float x, float y) {
    // 将像素坐标转换为格子坐标
    int grid_x = (int)(x / TILE_SIZE);
    int grid_y = (int)(y / TILE_SIZE);
    
    // 边界检查
    if (grid_x < 0 || grid_x >= MAP_WIDTH || grid_y < 0 || grid_y >= MAP_HEIGHT) {
        return 1; // 碰撞
    }
    
    // 检查方块类型
    BlockType block_type = get_block_type(grid_x, grid_y);
    if (block_type == BLOCK_NORMAL || block_type == BLOCK_REWARD || 
        block_type == BLOCK_GRASS || block_type == BLOCK_MUD) {
        return 1; // 碰撞
    }
    
    return 0; // 无碰撞
}

// 检查马里奥脚底是否碰到地面或平台
int check_ground_collision(float x, float y) {
    // 检查马里奥底部的碰撞（检查底部左右两个点）
    float bottom_y = y + mario.height;
    return check_collision(x, bottom_y) || 
           check_collision(x + mario.width - 1, bottom_y);
}

// 检查马里奥头顶是否碰到天花板
int check_ceiling_collision(float x, float y) {
    // 检查马里奥顶部的碰撞（检查顶部左右两个点）
    return check_collision(x, y) || 
           check_collision(x + mario.width - 1, y);
}

// 更新马里奥的水平速度（应用加速度和摩擦力）
void update_mario_horizontal_movement() {
    float friction = mario.on_ground ? GROUND_FRICTION : AIR_FRICTION;
    
    // 计算速度差
    float speed_diff = mario.target_vx - mario.vx;
    
    if (mario.target_vx != 0) {
        // 有目标速度时，向目标速度加速
        if (speed_diff > 0) {
            mario.vx += MARIO_ACCELERATION;
            if (mario.vx > mario.target_vx) {
                mario.vx = mario.target_vx;
            }
        } else if (speed_diff < 0) {
            mario.vx -= MARIO_ACCELERATION;
            if (mario.vx < mario.target_vx) {
                mario.vx = mario.target_vx;
            }
        }
    } else {
        // 没有目标速度时，应用摩擦力减速
        if (mario.vx > 0) {
            mario.vx -= friction;
            if (mario.vx < 0) mario.vx = 0;
        } else if (mario.vx < 0) {
            mario.vx += friction;
            if (mario.vx > 0) mario.vx = 0;
        }
    }
    
    // 限制最大速度
    if (mario.vx > MARIO_MAX_SPEED) mario.vx = MARIO_MAX_SPEED;
    if (mario.vx < -MARIO_MAX_SPEED) mario.vx = -MARIO_MAX_SPEED;
}

// 更新马里奥状态（每帧调用）
void update_mario() {
    if (!mario.alive) return;
    if (game_over) return;
    
    // 更新受伤无敌时间
    if (mario.hurt_timer > 0) {
        mario.hurt_timer -= 1.0f / 60.0f; // 假设60FPS
        if (mario.hurt_timer < 0) mario.hurt_timer = 0;
    }
    
    // 更新面向方向
    if (mario.vx > 0.1f) mario.facing_right = 1;
    else if (mario.vx < -0.1f) mario.facing_right = 0;
    
    // 更新动画状态
    MarioAnimationState new_anim_state;
    if (fabsf(mario.vx) > 0.1f) {
        new_anim_state = MARIO_ANIM_RUN;  // 移动时播放跑步动画
    } else {
        new_anim_state = MARIO_ANIM_IDLE; // 静止时播放静止动画
    }
    
    // 如果动画状态改变，重置动画
    if (new_anim_state != mario.anim_state) {
        mario.anim_state = new_anim_state;
        mario.anim_timer = 0.0f;
        mario.anim_frame = 0;
    }
    
    // 更新动画帧
    const float ANIM_SPEED = 0.1f; // 动画播放速度（每帧0.1秒）
    mario.anim_timer += 1.0f / 60.0f; // 假设60FPS
    
    if (mario.anim_timer >= ANIM_SPEED) {
        mario.anim_timer = 0.0f;
        
        // 获取当前动画的最大帧数
        int max_frames;
        if (mario.anim_state == MARIO_ANIM_IDLE) {
            max_frames = 4;  // idle有4帧
        } else if (mario.anim_state == MARIO_ANIM_RUN) {
            max_frames = 16; // run有16帧
        } else {
            max_frames = 1;
        }
        
        mario.anim_frame = (mario.anim_frame + 1) % max_frames;
    }
    
    // 更新水平移动（应用摩擦力和加速度）
    update_mario_horizontal_movement();
    
    // 应用重力（向下加速度）
    mario.vy += GRAVITY;
    
    // 限制最大下落速度
    if (mario.vy > MAX_FALL_SPEED) {
        mario.vy = MAX_FALL_SPEED;
    }
    
    // 水平移动处理
    float new_x = mario.x + mario.vx;
    
    // 检查水平移动碰撞
    int collision = 0;
    if (mario.vx > 0) {
        // 向右移动，检查右边界
        collision = check_collision(new_x + mario.width, mario.y) || 
                   check_collision(new_x + mario.width, mario.y + mario.height - 1);
    } else if (mario.vx < 0) {
        // 向左移动，检查左边界
        collision = check_collision(new_x, mario.y) || 
                   check_collision(new_x, mario.y + mario.height - 1);
    }
    
    if (!collision) {
        mario.x = new_x;
    } else {
        // 发生碰撞，停止移动
        if (mario.vx > 0) {
            // 向右撞墙
            int grid_x = (int)((new_x + mario.width) / TILE_SIZE);
            mario.x = (float)(grid_x * TILE_SIZE) - mario.width;
        } else if (mario.vx < 0) {
            // 向左撞墙
            int grid_x = (int)(new_x / TILE_SIZE);
            mario.x = (float)((grid_x + 1) * TILE_SIZE);
        }
        mario.vx = 0;
        mario.target_vx = 0;
    }
    
    // 垂直移动处理
    float new_y = mario.y + mario.vy;
    
    if (mario.vy > 0) {
        // 向下移动（下落）
        if (check_ground_collision(mario.x, new_y)) {
            // 着陆：将马里奥精确放置在地面上
            int grid_y = (int)((new_y + mario.height) / TILE_SIZE);
            mario.y = (float)(grid_y * TILE_SIZE) - mario.height;
            mario.vy = 0;
            mario.on_ground = 1;
        } else {
            mario.y = new_y;
            mario.on_ground = 0;
        }
    } else if (mario.vy < 0) {
        // 向上移动（跳跃）
        if (check_ceiling_collision(mario.x, new_y)) {
            // 撞天花板：马里奥头部精确贴住天花板
            int grid_y = (int)(new_y / TILE_SIZE);
            mario.y = (float)((grid_y + 1) * TILE_SIZE);
            mario.vy = 0;
            mario.on_ground = 0;
            
            // 检查是否撞击奖励方块
            check_block_collision();
        } else {
            mario.y = new_y;
            mario.on_ground = 0;
        }
    }
    
    // 检查是否掉出地图底部（死亡）
    if (mario.y > MAP_HEIGHT * TILE_SIZE) {
        mario_take_damage();
        printf("马里奥掉出地图！\n");
    }

    // 检查是否到达通关方块
    int mario_grid_x = (int)((mario.x + mario.width / 2) / TILE_SIZE);
    int mario_grid_y = (int)((mario.y + mario.height / 2) / TILE_SIZE);
    if (get_block_type(mario_grid_x, mario_grid_y) == BLOCK_GOAL) {
        game_over = 1;
        printf("恭喜通关！你成功到达终点！\n");
    }
}

// 马里奥跳跃
void mario_jump() {
    if (mario.on_ground && mario.alive) {
        mario.vy = JUMP_FORCE;
        mario.on_ground = 0;
    }
}

// 设置马里奥目标速度
void set_mario_target_velocity(float target_vx) {
    mario.target_vx = target_vx;
}

// 获取马里奥位置
void get_mario_position(float* x, float* y) {
    *x = mario.x;
    *y = mario.y;
}

// 获取马里奥尺寸
void get_mario_size(int* w, int* h) {
    *w = mario.width;
    *h = mario.height;
}

// 检查马里奥是否撞击奖励方块
int check_block_collision() {
    if (!mario.alive) return 0;
    
    // 计算马里奥头部中心的格子坐标
    float head_center_x = mario.x + mario.width / 2.0f;
    float head_y = mario.y;
    
    int grid_x = (int)(head_center_x / TILE_SIZE);
    int grid_y = (int)(head_y / TILE_SIZE);
    
    // 检查是否在地图范围内
    if (grid_x < 0 || grid_x >= MAP_WIDTH || grid_y < 0 || grid_y >= MAP_HEIGHT) {
        return 0;
    }
    
    // 检查是否是奖励方块
    if (game_map[grid_y][grid_x] == '?') {
        // 撞击奖励方块
        hit_block(grid_x, grid_y);
        printf("马里奥撞击奖励方块！\n");
        return 1;
    }
    
    return 0;
}

// 马里奥受伤
void mario_take_damage() {
    if (!mario.alive || mario.hurt_timer > 0) return; // 已死亡或无敌时不受伤
    
    mario.lives--;
    printf("马里奥受伤！剩余生命：%d\n", mario.lives);
    
    if (mario.lives <= 0) {
        mario.alive = 0;
        printf("马里奥死亡！游戏结束！\n");
    } else {
        // 设置无敌时间（2秒）
        mario.hurt_timer = 2.0f;
        
        // 重置马里奥位置
        mario.x = 2 * TILE_SIZE;
        mario.y = 6 * TILE_SIZE;
        mario.vx = 0;
        mario.vy = 0;
        mario.target_vx = 0;
        mario.on_ground = 0;
        
        printf("马里奥重生！无敌时间：2秒\n");
    }
}

// 检查马里奥是否处于无敌状态
int mario_is_invulnerable() {
    return mario.hurt_timer > 0;
}

// 获取马里奥生命数
int mario_get_lives() {
    return mario.lives;
}

// 动画相关接口实现
MarioAnimationState get_mario_animation_state() {
    return mario.anim_state;
}

int get_mario_animation_frame() {
    return mario.anim_frame;
}

int is_mario_facing_right() {
    return mario.facing_right;
} 
// mario.c
// 马里奥角色逻辑实现

#include "mario.h"
#include "map.h"
#include "blocks.h"
#include <stdio.h>
#include <math.h>

// 全局马里奥对象
Mario mario;

// 小马里奥的物理常量
#define MARIO_SMALL_MAX_SPEED 4.0f
#define MARIO_SMALL_ACCELERATION 0.8f
#define MARIO_SMALL_SIZE 32

// 大马里奥的物理常量
#define MARIO_BIG_MAX_SPEED 5.0f     // 稍微降低最大速度，减少抖动
#define MARIO_BIG_ACCELERATION 0.9f  // 稍微降低加速度，使移动更平滑
#define MARIO_BIG_WIDTH 32
#define MARIO_BIG_HEIGHT 48          // 大马里奥更高

// 通用物理常量
#define GROUND_FRICTION 0.15f
#define AIR_FRICTION 0.05f
#define GRAVITY 0.5f
#define JUMP_FORCE -12.0f
#define MAX_FALL_SPEED 10.0f
#define TILE_SIZE 32

// 初始化马里奥
void init_mario() {
    mario.x = 2 * TILE_SIZE;      // 初始位置（像素坐标）
    mario.y = 6 * TILE_SIZE;
    mario.vx = 0.0f;              // 初始速度为0
    mario.vy = 0.0f;
    mario.target_vx = 0.0f;       // 初始目标速度为0
    mario.width = MARIO_SMALL_SIZE;   // 小马里奥尺寸
    mario.height = MARIO_SMALL_SIZE;
    mario.alive = 1;              // 存活状态
    mario.on_ground = 0;          // 初始不在地面（会下落到地面）
    mario.state = MARIO_SMALL;    // 初始为小马里奥状态
    mario.lives = 3;              // 初始3条生命
    mario.hurt_timer = 0.0f;      // 初始无受伤状态
    mario.anim_state = MARIO_ANIM_IDLE; // 初始动画状态
    mario.facing_right = 1;       // 初始面向右
    mario.anim_timer = 0.0f;      // 初始动画计时器
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
    
    // 砖块和奖励方块碰撞检查
    char tile = game_map[grid_y][grid_x];
    if (tile == '#' || tile == '?') {
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
    
    // 根据马里奥状态获取对应的移动参数
    float max_speed = (mario.state == MARIO_BIG) ? MARIO_BIG_MAX_SPEED : MARIO_SMALL_MAX_SPEED;
    float acceleration = (mario.state == MARIO_BIG) ? MARIO_BIG_ACCELERATION : MARIO_SMALL_ACCELERATION;
    
    // 计算速度差
    float speed_diff = mario.target_vx - mario.vx;
    
    if (mario.target_vx != 0) {
        // 有目标速度时，向目标速度加速
        if (speed_diff > 0) {
            mario.vx += acceleration;
            if (mario.vx > mario.target_vx) {
                mario.vx = mario.target_vx;
            }
        } else if (speed_diff < 0) {
            mario.vx -= acceleration;
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
    if (mario.vx > max_speed) mario.vx = max_speed;
    if (mario.vx < -max_speed) mario.vx = -max_speed;
}

// 更新马里奥状态（每帧调用）
void update_mario() {
    if (!mario.alive) return;
    
    // 更新受伤无敌时间
    if (mario.hurt_timer > 0) {
        mario.hurt_timer -= 1.0f / 60.0f; // 假设60FPS
        if (mario.hurt_timer < 0) mario.hurt_timer = 0;
    }
    
    // 更新动画计时器
    mario.anim_timer += 1.0f / 60.0f;
    
    // 更新面向方向
    if (mario.vx > 0.1f) mario.facing_right = 1;
    else if (mario.vx < -0.1f) mario.facing_right = 0;
    
    // 更新动画状态
    if (!mario.on_ground) {
        mario.anim_state = MARIO_ANIM_JUMPING;
    } else if (fabsf(mario.vx) > 0.1f) {
        mario.anim_state = MARIO_ANIM_WALKING;
    } else {
        mario.anim_state = MARIO_ANIM_IDLE;
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
    
    // 检查水平移动碰撞（需要检查马里奥的四个角）
    int collision = 0;
    if (mario.vx > 0) {
        // 向右移动，检查真正的右前沿（修复"差一像素"bug）
        collision = check_collision(new_x + mario.width, mario.y) || 
                   check_collision(new_x + mario.width, mario.y + mario.height - 1);
    } else if (mario.vx < 0) {
        // 向左移动，检查左上角和左下角
        collision = check_collision(new_x, mario.y) || 
                   check_collision(new_x, mario.y + mario.height - 1);
    }
    
    if (!collision) {
        mario.x = new_x;
    } else {
        // 发生碰撞，将马里奥精确地移动到墙边
        if (mario.vx > 0) {
            // 向右撞墙，精确计算应该"贴"在墙边的位置
            int grid_x = (int)((new_x + mario.width) / TILE_SIZE);
            mario.x = (float)(grid_x * TILE_SIZE) - mario.width;
        } else if (mario.vx < 0) {
            // 向左撞墙
            int grid_x = (int)(new_x / TILE_SIZE);
            mario.x = (float)((grid_x + 1) * TILE_SIZE);
        }
        mario.vx = 0; // 碰撞时停止水平移动
        mario.target_vx = 0; // 同时清除目标速度
    }
    
    // 垂直移动处理
    float new_y = mario.y + mario.vy;
    
    if (mario.vy > 0) {
        // 向下移动（下落）
        if (check_ground_collision(mario.x, new_y)) {
            // 找到地面，停在地面上
            int grid_y = (int)((new_y + mario.height) / TILE_SIZE);
            mario.y = grid_y * TILE_SIZE - mario.height;
            mario.vy = 0;
            mario.on_ground = 1;
        } else {
            mario.y = new_y;
            mario.on_ground = 0;
        }
    } else if (mario.vy < 0) {
        // 向上移动（跳跃）
        
        if (check_ceiling_collision(mario.x, new_y)) {
            // 撞到天花板，检查是否是奖励方块
            int grid_y = (int)(new_y / TILE_SIZE) + 1;
            mario.y = grid_y * TILE_SIZE;
            mario.vy = 0;
            
            // 检查刚撞到的位置是否有奖励方块
            int map_x1 = (int)(mario.x / TILE_SIZE);
            int map_x2 = (int)((mario.x + mario.width - 1) / TILE_SIZE);
            int hit_map_y = (int)(mario.y / TILE_SIZE) - 1;  // 马里奥头顶上方一格
            
            // 只检查一次，避免重复撞击
            int hit_reward = 0;
            if (get_block_type(map_x1, hit_map_y) == BLOCK_REWARD && hit_block(map_x1, hit_map_y)) {
                hit_reward = 1;
            } else if (map_x2 != map_x1 && get_block_type(map_x2, hit_map_y) == BLOCK_REWARD && hit_block(map_x2, hit_map_y)) {
                hit_reward = 1;
            }
            
            if (hit_reward) {
                mario_upgrade();
            }
        } else {
            mario.y = new_y;
        }
        mario.on_ground = 0;
    }
}

// 马里奥跳跃
void mario_jump() {
    if (mario.on_ground && mario.alive) {
        mario.vy = JUMP_FORCE;
        mario.on_ground = 0;
    }
}

// 设置马里奥目标速度（用于摩擦力系统）
void set_mario_target_velocity(float target_vx) {
    mario.target_vx = target_vx;
}

// 获取马里奥位置
void get_mario_position(float* x, float* y) {
    if (x) *x = mario.x;
    if (y) *y = mario.y;
}

// 获取马里奥尺寸
void get_mario_size(int* w, int* h) {
    if (w) *w = mario.width;
    if (h) *h = mario.height;
}

// 马里奥升级（变大变快）
void mario_upgrade() {
    if (mario.state == MARIO_SMALL) {
        // 记录升级前的位置
        float old_height = mario.height;
        
        // 切换到大马里奥状态
        mario.state = MARIO_BIG;
        mario.width = MARIO_BIG_WIDTH;
        mario.height = MARIO_BIG_HEIGHT;
        
        // 调整位置，让马里奥的脚保持在同一水平线
        mario.y -= (mario.height - old_height);
        
        printf("马里奥升级！现在是大马里奥：更高更快！\n");
    }
}

// 获取马里奥当前状态
MarioState get_mario_state() {
    return mario.state;
}

// 检查是否撞击到奖励方块
int check_block_collision() {
    // 只在向上移动时检查
    if (mario.vy >= 0) return 0;
    
    // 检查马里奥头顶位置的方块
    float head_left_x = mario.x;                    // 头部左侧
    float head_right_x = mario.x + mario.width - 1; // 头部右侧
    float head_y = mario.y;                         // 头顶位置
    
    // 转换为地图坐标并检查
    int map_x1 = (int)(head_left_x / TILE_SIZE);
    int map_x2 = (int)(head_right_x / TILE_SIZE);
    int map_y = (int)(head_y / TILE_SIZE);
    
    // 检查左上角和右上角是否撞到奖励方块
    if (get_block_type(map_x1, map_y) == BLOCK_REWARD) {
        if (hit_block(map_x1, map_y)) {
            printf("撞击位置: (%d, %d)\n", map_x1, map_y);
            return 1;
        }
    }
    
    if (map_x2 != map_x1 && get_block_type(map_x2, map_y) == BLOCK_REWARD) {
        if (hit_block(map_x2, map_y)) {
            printf("撞击位置: (%d, %d)\n", map_x2, map_y);
            return 1;
        }
    }
    
    return 0;
}

// 马里奥受伤
void mario_take_damage() {
    if (mario.hurt_timer > 0) return; // 无敌时间内不受伤
    
    if (mario.state == MARIO_BIG) {
        // 大马里奥变回小马里奥
        mario.state = MARIO_SMALL;
        mario.width = MARIO_SMALL_SIZE;
        mario.height = MARIO_SMALL_SIZE;
        mario.hurt_timer = 2.0f; // 2秒无敌时间
        printf("马里奥变小了！\n");
    } else {
        // 小马里奥失去一条生命
        mario.lives--;
        mario.hurt_timer = 2.0f; // 2秒无敌时间
        
        if (mario.lives <= 0) {
            mario.alive = 0;
            printf("马里奥死亡！游戏结束！\n");
        } else {
            // 重置马里奥位置到起点
            mario.x = 2 * TILE_SIZE;
            mario.y = 6 * TILE_SIZE;
            mario.vx = 0;
            mario.vy = 0;
            mario.target_vx = 0;
            printf("马里奥失去一条生命！剩余生命：%d\n", mario.lives);
        }
    }
}

// 检查马里奥是否处于无敌状态
int mario_is_invulnerable() {
    return (mario.hurt_timer > 0);
}

// 获取马里奥生命数
int mario_get_lives() {
    return mario.lives;
}

// 获取马里奥当前应显示的精灵
SpriteID get_mario_sprite() {
    // 根据马里奥大小状态选择基础精灵组
    SpriteID base_sprite;
    
    if (mario.state == MARIO_BIG) {
        // 大马里奥
        switch (mario.anim_state) {
            case MARIO_ANIM_JUMPING:
                base_sprite = SPRITE_MARIO_BIG_JUMP;
                break;
            case MARIO_ANIM_WALKING:
                // 走路动画：每0.2秒切换一次帧
                base_sprite = ((int)(mario.anim_timer / 0.2f) % 2) ? 
                             SPRITE_MARIO_BIG_WALK1 : SPRITE_MARIO_BIG_WALK2;
                break;
            case MARIO_ANIM_IDLE:
            default:
                base_sprite = SPRITE_MARIO_BIG_IDLE;
                break;
        }
    } else {
        // 小马里奥
        switch (mario.anim_state) {
            case MARIO_ANIM_JUMPING:
                base_sprite = SPRITE_MARIO_SMALL_JUMP;
                break;
            case MARIO_ANIM_WALKING:
                // 走路动画：每0.2秒切换一次帧
                base_sprite = ((int)(mario.anim_timer / 0.2f) % 2) ? 
                             SPRITE_MARIO_SMALL_WALK1 : SPRITE_MARIO_SMALL_WALK2;
                break;
            case MARIO_ANIM_IDLE:
            default:
                base_sprite = SPRITE_MARIO_SMALL_IDLE;
                break;
        }
    }
    
    return base_sprite;
} 
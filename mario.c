// mario.c
// 马里奥角色逻辑实现

#include "mario.h"
#include "map.h"

// 全局马里奥对象
Mario mario;

// 移动和物理常量
#define MARIO_MAX_SPEED 4.0f     // 最大移动速度
#define MARIO_ACCELERATION 0.8f  // 地面加速度
#define GROUND_FRICTION 0.15f    // 地面摩擦力
#define AIR_FRICTION 0.05f       // 空气阻力
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
    mario.width = TILE_SIZE;      // 马里奥尺寸
    mario.height = TILE_SIZE;
    mario.alive = 1;              // 存活状态
    mario.on_ground = 0;          // 初始不在地面（会下落到地面）
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
    
    // 砖块碰撞检查
    if (game_map[grid_y][grid_x] == '#') {
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
        // 向右移动，检查右上角和右下角
        collision = check_collision(new_x + mario.width - 1, mario.y) || 
                   check_collision(new_x + mario.width - 1, mario.y + mario.height - 1);
    } else if (mario.vx < 0) {
        // 向左移动，检查左上角和左下角
        collision = check_collision(new_x, mario.y) || 
                   check_collision(new_x, mario.y + mario.height - 1);
    }
    
    if (!collision) {
        mario.x = new_x;
    } else {
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
            // 撞到天花板
            int grid_y = (int)(new_y / TILE_SIZE) + 1;
            mario.y = grid_y * TILE_SIZE;
            mario.vy = 0;
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
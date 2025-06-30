// enemy.c
// 敌人系统实现

#include "enemy.h"
#include "map.h"
#include "mario.h"
#include <stdio.h>
#include <math.h>

// 全局敌人数组
Enemy enemies[MAX_ENEMIES];
int enemy_count = 0;

// 敌人物理常量
#define TILE_SIZE 16
#define GRAVITY 0.5f
#define MAX_FALL_SPEED 10.0f

// 栗子小子常量
#define GOOMBA_SPEED 1.0f
#define GOOMBA_WIDTH 16
#define GOOMBA_HEIGHT 16
#define DEATH_ANIMATION_TIME 1.0f  // 死亡动画持续时间（秒）

// 初始化敌人系统
void init_enemies() {
    enemy_count = 0;
    
    // 在地图中预设一些敌人位置（调整为更近的位置方便测试）
    add_enemy(ENEMY_GOOMBA, 8 * TILE_SIZE, 6 * TILE_SIZE);   // 第8列
    add_enemy(ENEMY_GOOMBA, 12 * TILE_SIZE, 6 * TILE_SIZE);  // 第12列
    add_enemy(ENEMY_GOOMBA, 20 * TILE_SIZE, 6 * TILE_SIZE);  // 第20列
    add_enemy(ENEMY_GOOMBA, 30 * TILE_SIZE, 6 * TILE_SIZE);  // 第30列
    
    printf("敌人系统初始化完成，创建了%d个敌人\n", enemy_count);
}

// 添加敌人
void add_enemy(EnemyType type, float x, float y) {
    if (enemy_count >= MAX_ENEMIES) {
        printf("警告：敌人数量已达到最大值！\n");
        return;
    }
    
    Enemy* enemy = &enemies[enemy_count];
    enemy->x = x;
    enemy->y = y;
    enemy->vx = 0;
    enemy->vy = 0;
    enemy->type = type;
    enemy->state = ENEMY_STATE_ALIVE;
    enemy->alive = 1;
    enemy->direction = -1;  // 默认向左移动
    enemy->death_timer = 0;
    enemy->on_ground = 0;
    
    // 根据敌人类型设置属性
    switch (type) {
        case ENEMY_GOOMBA:
            enemy->width = GOOMBA_WIDTH;
            enemy->height = GOOMBA_HEIGHT;
            enemy->vx = -GOOMBA_SPEED;  // 向左移动
            break;
        default:
            enemy->width = GOOMBA_WIDTH;
            enemy->height = GOOMBA_HEIGHT;
            break;
    }
    
    enemy_count++;
}

// 检查敌人碰撞（复用地图碰撞检测逻辑）
int check_enemy_collision(Enemy* enemy, float new_x, float new_y) {
    // 将像素坐标转换为格子坐标
    int grid_x = (int)(new_x / TILE_SIZE);
    int grid_y = (int)(new_y / TILE_SIZE);
    
    // 边界检查
    if (grid_x < 0 || grid_x >= MAP_WIDTH || grid_y < 0 || grid_y >= MAP_HEIGHT) {
        return 1; // 碰撞
    }
    
    // 砖块碰撞检查
    char tile = game_map[grid_y][grid_x];
    if (tile == '#' || tile == '?') {
        return 1; // 碰撞
    }
    
    return 0; // 无碰撞
}

// 检查敌人脚底是否碰到地面
int check_enemy_ground_collision(Enemy* enemy, float x, float y) {
    float bottom_y = y + enemy->height;
    return check_enemy_collision(enemy, x, bottom_y) || 
           check_enemy_collision(enemy, x + enemy->width - 1, bottom_y);
}

// 更新敌人物理
void update_enemy_physics(Enemy* enemy) {
    if (!enemy->alive || enemy->state == ENEMY_STATE_DEAD) return;
    
    // 应用重力
    enemy->vy += GRAVITY;
    if (enemy->vy > MAX_FALL_SPEED) {
        enemy->vy = MAX_FALL_SPEED;
    }
    
    // 水平移动处理
    float new_x = enemy->x + enemy->vx;
    
    // 检查水平碰撞
    int collision = 0;
    if (enemy->vx > 0) {
        // 向右移动
        collision = check_enemy_collision(enemy, new_x + enemy->width, enemy->y) || 
                   check_enemy_collision(enemy, new_x + enemy->width, enemy->y + enemy->height - 1);
    } else if (enemy->vx < 0) {
        // 向左移动
        collision = check_enemy_collision(enemy, new_x, enemy->y) || 
                   check_enemy_collision(enemy, new_x, enemy->y + enemy->height - 1);
    }
    
    // 检查悬崖边缘（防止敌人掉下悬崖）
    int cliff_ahead = 0;
    if (enemy->vx > 0) {
        // 向右移动时，检查右前方是否有地面
        cliff_ahead = !check_enemy_ground_collision(enemy, new_x + enemy->width, enemy->y);
    } else if (enemy->vx < 0) {
        // 向左移动时，检查左前方是否有地面
        cliff_ahead = !check_enemy_ground_collision(enemy, new_x - 1, enemy->y);
    }
    
    if (!collision && !cliff_ahead) {
        enemy->x = new_x;
    } else {
        // 撞墙或遇到悬崖时转向
        enemy->direction *= -1;
        enemy->vx *= -1;
    }
    
    // 垂直移动处理
    float new_y = enemy->y + enemy->vy;
    
    if (enemy->vy > 0) {
        // 向下移动（下落）
        if (check_enemy_ground_collision(enemy, enemy->x, new_y)) {
            // 找到地面
            int grid_y = (int)((new_y + enemy->height) / TILE_SIZE);
            enemy->y = grid_y * TILE_SIZE - enemy->height;
            enemy->vy = 0;
            enemy->on_ground = 1;
        } else {
            enemy->y = new_y;
            enemy->on_ground = 0;
        }
    } else if (enemy->vy < 0) {
        // 向上移动（跳跃，虽然栗子小子通常不跳跃）
        if (check_enemy_collision(enemy, enemy->x, new_y) || 
            check_enemy_collision(enemy, enemy->x + enemy->width - 1, new_y)) {
            enemy->vy = 0;
        } else {
            enemy->y = new_y;
            enemy->on_ground = 0;
        }
    }
}

// 更新敌人AI
void update_enemy_ai(Enemy* enemy) {
    if (!enemy->alive) return;
    
    switch (enemy->state) {
        case ENEMY_STATE_ALIVE:
            // 栗子小子简单AI：直线移动
            if (enemy->type == ENEMY_GOOMBA) {
                enemy->vx = enemy->direction * GOOMBA_SPEED;
            }
            break;
            
        case ENEMY_STATE_STOMPED:
            // 被踩死状态：停止移动，播放死亡动画
            enemy->vx = 0;
            enemy->death_timer += 1.0f / 60.0f; // 假设60FPS
            
            if (enemy->death_timer >= DEATH_ANIMATION_TIME) {
                enemy->state = ENEMY_STATE_DEAD;
                enemy->alive = 0;
            }
            break;
            
        case ENEMY_STATE_DEAD:
            // 死亡状态：什么都不做
            break;
    }
}

// 更新所有敌人
void update_enemies() {
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].alive) {
            update_enemy_ai(&enemies[i]);
            update_enemy_physics(&enemies[i]);
        }
    }
}

// 踩死敌人
void stomp_enemy(int enemy_index) {
    if (enemy_index < 0 || enemy_index >= enemy_count) return;
    
    Enemy* enemy = &enemies[enemy_index];
    if (enemy->state == ENEMY_STATE_ALIVE) {
        enemy->state = ENEMY_STATE_STOMPED;
        enemy->death_timer = 0;
        
        // 可以在这里添加得分逻辑
        printf("踩死了一个敌人！\n");
    }
}

// 杀死敌人（其他方式，如火球）
void kill_enemy(int enemy_index) {
    if (enemy_index < 0 || enemy_index >= enemy_count) return;
    
    Enemy* enemy = &enemies[enemy_index];
    enemy->state = ENEMY_STATE_DEAD;
    enemy->alive = 0;
}

// 检查马里奥与敌人的碰撞
int check_mario_enemy_collision() {
    // 如果马里奥处于无敌状态，不检查碰撞
    if (mario_is_invulnerable()) return 0;
    
    float mario_x, mario_y;
    int mario_w, mario_h;
    get_mario_position(&mario_x, &mario_y);
    get_mario_size(&mario_w, &mario_h);
    
    for (int i = 0; i < enemy_count; i++) {
        Enemy* enemy = &enemies[i];
        
        if (!enemy->alive || enemy->state != ENEMY_STATE_ALIVE) continue;
        
        // 简单的矩形碰撞检测
        if (mario_x < enemy->x + enemy->width &&
            mario_x + mario_w > enemy->x &&
            mario_y < enemy->y + enemy->height &&
            mario_y + mario_h > enemy->y) {
            
            // 检查是否是从上方踩踏
            float mario_bottom = mario_y + mario_h;
            float enemy_top = enemy->y;
            
            // 如果马里奥的底部接近敌人的顶部，并且马里奥在下降或接近地面
            if (mario_bottom <= enemy_top + 10 && mario_bottom >= enemy_top - 4) {
                // 踩踏敌人
                stomp_enemy(i);
                
                // 让马里奥弹跳一下
                mario.vy = -6.0f; // 小幅弹跳
                
                return 0; // 不伤害马里奥
            } else {
                // 侧面碰撞，马里奥受伤
                return 1; // 返回1表示马里奥受伤
            }
        }
    }
    
    return 0; // 无碰撞
}

// 移除死亡的敌人（压缩数组）
void remove_dead_enemies() {
    int write_index = 0;
    
    for (int read_index = 0; read_index < enemy_count; read_index++) {
        if (enemies[read_index].alive || enemies[read_index].state != ENEMY_STATE_DEAD) {
            if (write_index != read_index) {
                enemies[write_index] = enemies[read_index];
            }
            write_index++;
        }
    }
    
    enemy_count = write_index;
}

// 获取敌人信息（用于渲染）
void get_enemy_info(int index, float* x, float* y, int* w, int* h, EnemyState* state) {
    if (index < 0 || index >= enemy_count) return;
    
    Enemy* enemy = &enemies[index];
    if (x) *x = enemy->x;
    if (y) *y = enemy->y;
    if (w) *w = enemy->width;
    if (h) *h = enemy->height;
    if (state) *state = enemy->state;
}

// 获取活着的敌人数量
int get_alive_enemy_count() {
    int count = 0;
    for (int i = 0; i < enemy_count; i++) {
        if (enemies[i].alive) count++;
    }
    return count;
} 
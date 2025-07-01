// enemy.h
// 敌人系统头文件

#ifndef ENEMY_H
#define ENEMY_H

#include "map.h"

// 敌人类型枚举
typedef enum {
    ENEMY_GOOMBA,    // 栗子小子
    ENEMY_KOOPA,     // 乌龟（暂未实现）
    ENEMY_TYPES_COUNT
} EnemyType;

// 敌人状态枚举
typedef enum {
    ENEMY_STATE_ALIVE,      // 存活
    ENEMY_STATE_STOMPED,    // 被踩死
    ENEMY_STATE_DEAD        // 死亡
} EnemyState;

// 敌人动画状态枚举
typedef enum {
    ENEMY_ANIM_IDLE,        // 默认动画
    ENEMY_ANIM_HIT          // 受击动画
} EnemyAnimationState;

// 敌人结构体
typedef struct {
    float x, y;              // 世界坐标位置
    float vx, vy;            // 速度
    int width, height;       // 尺寸
    EnemyType type;          // 敌人类型
    EnemyState state;        // 敌人状态
    int alive;               // 是否存活（1=存活，0=死亡）
    int direction;           // 移动方向（-1=左，1=右）
    float death_timer;       // 死亡动画计时器
    int on_ground;           // 是否在地面上
    
    // 动画相关
    EnemyAnimationState anim_state;  // 当前动画状态
    float anim_timer;                // 动画计时器
    int anim_frame;                  // 当前动画帧
    int is_taking_damage;            // 是否正在受击
    float hit_timer;                 // 受击状态计时器
} Enemy;

// 敌人数组和数量
#define MAX_ENEMIES 20
extern Enemy enemies[MAX_ENEMIES];
extern int enemy_count;

// 函数声明
void init_enemies();                                    // 初始化敌人系统
void add_enemy(EnemyType type, float x, float y);      // 添加敌人
void update_enemies();                                  // 更新所有敌人
void remove_dead_enemies();                             // 移除死亡的敌人
int check_knight_enemy_collision();                    // 检查骑士与敌人的碰撞
void stomp_enemy(int enemy_index);                     // 踩死敌人
void kill_enemy(int enemy_index);                      // 杀死敌人

// 敌人物理和AI函数
void update_enemy_physics(Enemy* enemy);               // 更新敌人物理
void update_enemy_ai(Enemy* enemy);                    // 更新敌人AI
int check_enemy_collision(Enemy* enemy, float new_x, float new_y); // 检查敌人碰撞

// 获取敌人信息函数
void get_enemy_info(int index, float* x, float* y, int* w, int* h, EnemyState* state);
int get_alive_enemy_count();                           // 获取活着的敌人数量

// 获取敌人动画信息函数
EnemyAnimationState get_enemy_animation_state(int index);  // 获取敌人动画状态
int get_enemy_animation_frame(int index);                  // 获取敌人动画帧
int get_enemy_direction(int index);                        // 获取敌人面向方向

#endif // ENEMY_H 
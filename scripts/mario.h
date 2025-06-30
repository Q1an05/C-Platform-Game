// mario.h
// 马里奥角色相关头文件，定义马里奥属性和接口

#ifndef MARIO_H
#define MARIO_H

// 马里奥结构体定义
typedef struct {
    float x, y;          // 像素坐标位置
    float vx, vy;        // 水平和垂直速度
    float target_vx;     // 目标水平速度（用于摩擦力计算）
    int width, height;   // 马里奥的宽高（像素，固定16x16）
    int alive;           // 是否存活（1=存活，0=死亡）
    int on_ground;       // 是否在地面上（1=在地面，0=在空中）
    int lives;           // 生命数量
    float hurt_timer;    // 受伤无敌时间
    int facing_right;    // 面向方向（1=右，0=左）
} Mario;

// 全局马里奥对象（允许外部模块访问）
extern Mario mario;

// 马里奥相关函数接口
void init_mario();                    // 初始化马里奥
void update_mario();                  // 更新马里奥状态（位置、碰撞等）
void set_mario_target_velocity(float target_vx);  // 设置马里奥目标速度
void mario_jump();                    // 马里奥跳跃
void get_mario_position(float* x, float* y);  // 获取马里奥位置
void get_mario_size(int* w, int* h);          // 获取马里奥尺寸

// 碰撞检测接口
int check_block_collision();    // 检查是否撞击到奖励方块

// 伤害系统接口
void mario_take_damage();       // 马里奥受伤
int mario_is_invulnerable();    // 检查马里奥是否处于无敌状态
int mario_get_lives();          // 获取马里奥生命数

#endif // MARIO_H 
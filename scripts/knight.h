// knight.h
// 骑士角色相关头文件，定义骑士属性和接口

#ifndef KNIGHT_H
#define KNIGHT_H


// 骑士物理常量
#define KNIGHT_ACCELERATION 0.35f
#define KNIGHT_WIDTH 15             // 宽度15像素
#define KNIGHT_HEIGHT 20            // 高度20像素

// 通用物理常量
#define GROUND_FRICTION 0.14f
#define AIR_FRICTION 0.05f
#define GRAVITY 0.4f
#define JUMP_FORCE -7.0f
#define MAX_FALL_SPEED 10.0f
#define TILE_SIZE 16
#define KNIGHT_MAX_SPEED 2.2f
#define DASH_SPEED 6.0f

// 骑士动画状态枚举
typedef enum {
    KNIGHT_ANIM_IDLE,    // 静止动画
    KNIGHT_ANIM_RUN,     // 跑步动画
    KNIGHT_ANIM_HIT,     // 受击动画
    KNIGHT_ANIM_DEATH    // 死亡动画
} KnightAnimationState;

// 骑士结构体定义
typedef struct {
    float x, y;          // 像素坐标位置
    float vx, vy;        // 水平和垂直速度
    float target_vx;     // 目标水平速度（用于摩擦力计算）
    int width, height;   // 骑士的宽高（像素，固定15x20）
    int alive;           // 是否存活（1=存活，0=死亡）
    int on_ground;       // 是否在地面上（1=在地面，0=在空中）
    int lives;           // 生命数量
    float hurt_timer;    // 受伤无敌时间
    int facing_right;    // 面向方向（1=右，0=左）
    
    // 动画相关
    KnightAnimationState anim_state;  // 当前动画状态
    float anim_timer;                 // 动画计时器
    int anim_frame;                   // 当前动画帧
    
    // 状态相关
    int is_taking_damage;  // 是否正在受击（播放受击动画）
    int is_dying;          // 是否正在死亡（播放死亡动画）
    float state_timer;     // 状态计时器（用于控制受击/死亡动画时长）
    
    // 二连跳相关
    int can_double_jump;   // 是否获得二连跳能力
    int double_jump_used;  // 本次空中是否已用过二连跳
    // 冲刺相关
    int can_dash;          // 是否获得冲刺能力
    int is_dashing;        // 当前是否正在冲刺
    float dash_timer;      // 冲刺剩余时间
    float dash_cooldown;   // 冲刺冷却剩余时间
} Knight;

// 全局骑士对象（允许外部模块访问）
extern Knight knight;

// 骑士相关函数接口
void init_knight();                    // 初始化骑士
void update_knight();                  // 更新骑士状态（位置、碰撞等）
void set_knight_target_velocity(float target_vx);  // 设置骑士目标速度
void knight_jump();                    // 骑士跳跃
void get_knight_position(float* x, float* y);  // 获取骑士位置
void get_knight_size(int* w, int* h);          // 获取骑士尺寸

// 动画相关接口
KnightAnimationState get_knight_animation_state(); // 获取当前动画状态
int get_knight_animation_frame();                  // 获取当前动画帧
int is_knight_facing_right();                     // 获取面向方向

// 碰撞检测接口
int check_block_collision();    // 检查是否撞击到奖励方块

// 伤害系统接口
void knight_take_damage();       // 骑士受伤
int knight_is_invulnerable();    // 检查骑士是否处于无敌状态
int knight_get_lives();          // 获取骑士生命数

// 运动与碰撞检测接口
int check_collision(float x, float y); // 检查指定位置是否有碰撞
int check_ground_collision(float x, float y); // 检查骑士脚底是否碰到地面
int check_ceiling_collision(float x, float y); // 检查骑士头顶是否碰到天花板
void update_knight_horizontal_movement(); // 更新骑士的水平速度

void knight_enable_double_jump(); // 获得二连跳能力
void knight_enable_dash();        // 获得冲刺能力
void knight_dash();                // 执行冲刺

#endif // KNIGHT_H 
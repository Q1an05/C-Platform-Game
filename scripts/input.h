// input.h
// 输入处理相关头文件，定义输入动作和接口

#ifndef INPUT_H
#define INPUT_H

#include <SDL.h>

// 游戏输入动作枚举
typedef enum {
    INPUT_LEFT,      // 向左移动
    INPUT_RIGHT,     // 向右移动
    INPUT_JUMP,      // 跳跃
    INPUT_PAUSE,     // 暂停游戏
    INPUT_QUIT,      // 退出游戏
    INPUT_DASH,      // 冲刺
    INPUT_COUNT       // 输入动作总数（用于数组大小）
} InputAction;

// 输入处理接口
void init_input();                              // 初始化输入系统
void update_input(SDL_Event* event);           // 更新输入状态（处理SDL事件）
void update_input_frame();                     // 更新帧状态（每帧调用一次）
int is_action_pressed(InputAction action);     // 查询动作是否被按下
int is_action_just_pressed(InputAction action); // 查询动作是否刚被按下
int is_action_released(InputAction action);    // 查询动作是否刚被释放
void cleanup_input();                          // 清理输入系统
void process_input();

#endif // INPUT_H 
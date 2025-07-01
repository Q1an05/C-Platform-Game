// input.c
// 输入处理模块实现

#include "input.h"
#include "knight.h"
#include "map.h"

// 键盘状态数组
static int current_keys[INPUT_COUNT];   // 当前帧的按键状态
static int previous_keys[INPUT_COUNT];  // 上一帧的按键状态

// 按键映射表：将SDL键码映射到游戏动作
static SDL_Keycode key_mapping[INPUT_COUNT] = {
    [INPUT_LEFT]  = SDLK_LEFT,     // 左方向键
    [INPUT_RIGHT] = SDLK_RIGHT,    // 右方向键
    [INPUT_JUMP]  = SDLK_SPACE,    // 空格键
    [INPUT_PAUSE] = SDLK_p,        // P键暂停
    [INPUT_QUIT]  = SDLK_ESCAPE,   // ESC键退出
    [INPUT_DASH]  = SDLK_d         // D键冲刺
};

extern Knight knight;

// 初始化输入系统
void init_input() {
    // 清空所有按键状态
    for (int i = 0; i < INPUT_COUNT; i++) {
        current_keys[i] = 0;
        previous_keys[i] = 0;
    }
}

// 根据SDL键码查找对应的游戏动作
static InputAction find_action_by_key(SDL_Keycode key) {
    for (int i = 0; i < INPUT_COUNT; i++) {
        if (key_mapping[i] == key) {
            return (InputAction)i;
        }
    }
    return INPUT_COUNT; // 未找到对应动作
}

// 更新输入状态（处理单个SDL事件）
void update_input(SDL_Event* event) {
    if (!event) return;
    
    // 处理键盘事件
    if (event->type == SDL_KEYDOWN) {
        InputAction action = find_action_by_key(event->key.keysym.sym);
        if (action < INPUT_COUNT) {
            current_keys[action] = 1;
        }
    } else if (event->type == SDL_KEYUP) {
        InputAction action = find_action_by_key(event->key.keysym.sym);
        if (action < INPUT_COUNT) {
            current_keys[action] = 0;
        }
    }
}

// 更新帧状态（每帧调用一次，在处理完所有事件后）
void update_input_frame() {
    // 保存上一帧的按键状态
    for (int i = 0; i < INPUT_COUNT; i++) {
        previous_keys[i] = current_keys[i];
    }
}

// 查询动作是否被按下（持续按下状态）
int is_action_pressed(InputAction action) {
    if (action >= INPUT_COUNT) return 0;
    return current_keys[action];
}

// 查询动作是否刚被按下（按下的瞬间）
int is_action_just_pressed(InputAction action) {
    if (action >= INPUT_COUNT) return 0;
    return current_keys[action] && !previous_keys[action];
}

// 查询动作是否刚被释放（释放的瞬间）
int is_action_released(InputAction action) {
    if (action >= INPUT_COUNT) return 0;
    return !current_keys[action] && previous_keys[action];
}

// 清理输入系统
void cleanup_input() {
    // 当前实现不需要特殊清理
    // 可以在这里释放资源或重置状态
}

void process_input() {
    float speed = KNIGHT_MAX_SPEED;
    // 移动输入
    if (!knight.is_dashing) {
        if (is_action_pressed(INPUT_LEFT)) {
            set_knight_target_velocity(-speed);
        } else if (is_action_pressed(INPUT_RIGHT)) {
            set_knight_target_velocity(speed);
        } else {
            set_knight_target_velocity(0);
        }
    }
    // 跳跃输入
    if (is_action_just_pressed(INPUT_JUMP)) {
        knight_jump();
    }
    // 冲刺输入
    if (is_action_just_pressed(INPUT_DASH)) {
        knight_dash();
    }
} 
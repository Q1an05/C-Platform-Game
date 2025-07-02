// knight.c
// 骑士角色逻辑实现

#include "knight.h"
#include "map.h"
#include "blocks.h"
#include "ui.h"
#include "sound.h"
#include <stdio.h>
#include <math.h>
#include "camera.h"

// 全局骑士对象
Knight knight;

extern int game_over;

// 存档点坐标
static float save_x = 2 * TILE_SIZE;
static float save_y = 6 * TILE_SIZE;
static int save_set = 0; // 是否已存档（保留用于初始化检查）
static int on_save_block = 0; // 当前是否在存档点上（用于避免重复触发）
static int game_won = 0; // 是否已通关（防止重复触发）

// 初始化骑士
void init_knight() {
    knight.x = 2 * TILE_SIZE;      // 初始位置（像素坐标）
    knight.y = 6 * TILE_SIZE;
    knight.vx = 0.0f;              // 初始速度为0
    knight.vy = 0.0f;
    knight.target_vx = 0.0f;       // 初始目标速度为0
    knight.width = KNIGHT_WIDTH;    // 固定15x20尺寸
    knight.height = KNIGHT_HEIGHT;
    knight.alive = 1;              // 存活状态
    knight.on_ground = 0;          // 初始不在地面（会下落到地面）
    knight.lives = 3;              // 初始3条生命
    knight.hurt_timer = 0.0f;      // 初始无受伤状态
    knight.facing_right = 1;       // 初始面向右
    
    // 初始化动画状态
    knight.anim_state = KNIGHT_ANIM_IDLE;
    knight.anim_timer = 0.0f;
    knight.anim_frame = 0;
    
    // 初始化状态
    knight.is_taking_damage = 0;
    knight.is_dying = 0;
    knight.state_timer = 0.0f;
    knight.can_double_jump = 0;
    knight.double_jump_used = 0;
    knight.can_dash = 0;
    knight.is_dashing = 0;
    knight.dash_timer = 0.0f;
    knight.dash_cooldown = 0.0f;
    
    // 重置游戏标志和存档状态
    game_won = 0;
    on_save_block = 0; // 重置存档点状态
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
    if (block_type == BLOCK_NORMAL || 
        block_type == BLOCK_GRASS || block_type == BLOCK_MUD) {
        return 1; // 碰撞
    }
    // 敌人屏障对骑士不产生碰撞
    if (block_type == BLOCK_ENEMY_BARRIER) {
        return 0; // 骑士可以通过
    }
    
    return 0; // 无碰撞
}

// 检查骑士脚底是否碰到地面或平台
int check_ground_collision(float x, float y) {
    // 检查骑士底部的碰撞（检查底部左右两个点）
    float bottom_y = y + knight.height;
    return check_collision(x, bottom_y) || 
           check_collision(x + knight.width - 1, bottom_y);
}

// 检查骑士头顶是否碰到天花板
int check_ceiling_collision(float x, float y) {
    // 检查骑士顶部的碰撞（检查顶部左右两个点）
    return check_collision(x, y) || 
           check_collision(x + knight.width - 1, y);
}

// 更新骑士的水平速度（简化的加速度逻辑）
void update_knight_horizontal_movement() {
    float friction = knight.on_ground ? GROUND_FRICTION : AIR_FRICTION;
    
    if (knight.target_vx > 0) {
        // 按下右键：向右加速，但不超过目标速度
        if (knight.vx < knight.target_vx) {
            knight.vx += KNIGHT_ACCELERATION;
        }
        else {
            knight.vx -= friction;
        }
        // 如果当前速度已经大于等于目标速度，摩擦力减速
    } else if (knight.target_vx < 0) {
        // 按下左键：向左加速，但不超过目标速度（绝对值）
        if (knight.vx > knight.target_vx) {
            knight.vx -= KNIGHT_ACCELERATION;
        }
        else {
            knight.vx += friction;
        }
        // 如果当前速度已经小于等于目标速度，摩擦力减速
    } else {
        // 没有按方向键：应用摩擦力减速
        if (knight.vx > 0) {
            knight.vx -= friction;
            if (knight.vx < 0) knight.vx = 0;
        } else if (knight.vx < 0) {
            knight.vx += friction;
            if (knight.vx > 0) knight.vx = 0;
        }
    }
}

// 更新骑士状态（每帧调用）
void update_knight() {
    if (!knight.alive) return;
    if (game_over) return;
    
    // 更新受伤无敌时间
    if (knight.hurt_timer > 0) {
        knight.hurt_timer -= 1.0f / 60.0f; // 假设60FPS
        if (knight.hurt_timer < 0) knight.hurt_timer = 0;
    }
    
    // 更新面向方向
    if (knight.vx > 0.1f) knight.facing_right = 1;
    else if (knight.vx < -0.1f) knight.facing_right = 0;
    
    // 更新状态计时器
    if (knight.state_timer > 0) {
        knight.state_timer -= 1.0f / 60.0f; // 假设60FPS
        if (knight.state_timer <= 0) {
            knight.state_timer = 0.0f;
            // 状态结束，重置状态标志
            if (knight.is_taking_damage) {
                knight.is_taking_damage = 0;
            }
            if (knight.is_dying) {
                // 死亡动画播放完毕，但保持死亡动画状态和最后一帧
                knight.alive = 0; // 设置为死亡，但不重置is_dying标志
            }
        }
    }
    
    // 更新动画状态
    KnightAnimationState new_anim_state;
    if (knight.is_dying) {
        new_anim_state = KNIGHT_ANIM_DEATH; // 死亡动画优先级最高
    } else if (knight.is_taking_damage) {
        new_anim_state = KNIGHT_ANIM_HIT;   // 受击动画
    } else if (fabsf(knight.vx) > 0.1f) {
        new_anim_state = KNIGHT_ANIM_RUN;   // 移动时播放跑步动画
    } else {
        new_anim_state = KNIGHT_ANIM_IDLE;  // 静止时播放静止动画
    }
    
    // 如果动画状态改变，重置动画
    if (new_anim_state != knight.anim_state) {
        knight.anim_state = new_anim_state;
        knight.anim_timer = 0.0f;
        knight.anim_frame = 0;
    }
    
    // 更新动画帧
    const float ANIM_SPEED = 0.1f; // 动画播放速度（每帧0.1秒）
    knight.anim_timer += 1.0f / 60.0f; // 假设60FPS
    
    if (knight.anim_timer >= ANIM_SPEED) {
        knight.anim_timer = 0.0f;
        
        // 获取当前动画的最大帧数
        int max_frames;
        if (knight.anim_state == KNIGHT_ANIM_IDLE) {
            max_frames = 4;  // idle有4帧
        } else if (knight.anim_state == KNIGHT_ANIM_RUN) {
            max_frames = 16; // run有16帧
        } else if (knight.anim_state == KNIGHT_ANIM_HIT) {
            max_frames = 4;  // hit有4帧
        } else if (knight.anim_state == KNIGHT_ANIM_DEATH) {
            max_frames = 4;  // death有4帧
        } else {
            max_frames = 1;
        }
        
        // 对于死亡和受击动画，只播放一次
        if (knight.anim_state == KNIGHT_ANIM_DEATH || knight.anim_state == KNIGHT_ANIM_HIT) {
            // 只有在动画还没播放完时才推进帧数
            if (knight.anim_frame < max_frames - 1) {
                knight.anim_frame++;
            }
            // 如果已经到达最后一帧，就停留在最后一帧，不再改变
        } else {
            knight.anim_frame = (knight.anim_frame + 1) % max_frames; // 循环播放
        }
    }
    
    // 更新水平移动（应用摩擦力和加速度）
    update_knight_horizontal_movement();
    
    // 应用重力（向下加速度）
    knight.vy += GRAVITY;
    
    // 限制最大下落速度
    if (knight.vy > MAX_FALL_SPEED) {
        knight.vy = MAX_FALL_SPEED;
    }
    
    // 水平移动处理
    float new_x = knight.x + knight.vx;
    
    // 检查水平移动碰撞
    int collision = 0;
    if (knight.vx > 0) {
        // 向右移动，检查右边界
        collision = check_collision(new_x + knight.width, knight.y) || 
                   check_collision(new_x + knight.width, knight.y + knight.height - 1);
    } else if (knight.vx < 0) {
        // 向左移动，检查左边界
        collision = check_collision(new_x, knight.y) || 
                   check_collision(new_x, knight.y + knight.height - 1);
    }
    
    if (!collision) {
        knight.x = new_x;
    } else {
        // 发生碰撞，停止移动
        if (knight.vx > 0) {
            // 向右撞墙
            int grid_x = (int)((new_x + knight.width) / TILE_SIZE);
            knight.x = (float)(grid_x * TILE_SIZE) - knight.width;
        } else if (knight.vx < 0) {
            // 向左撞墙
            int grid_x = (int)(new_x / TILE_SIZE);
            knight.x = (float)((grid_x + 1) * TILE_SIZE);
        }
        knight.vx = 0;
        knight.target_vx = 0;
    }
    
    // 垂直移动处理
    float new_y = knight.y + knight.vy;
    
    if (knight.vy > 0) {
        // 向下移动（下落）
        if (check_ground_collision(knight.x, new_y)) {
            // 着陆：将骑士精确放置在地面上
            int grid_y = (int)((new_y + knight.height) / TILE_SIZE);
            knight.y = (float)(grid_y * TILE_SIZE) - knight.height;
            knight.vy = 0;
            knight.on_ground = 1;
            knight.double_jump_used = 0;
            knight.is_dashing = 0;
            knight.dash_timer = 0.0f;
        } else {
            knight.y = new_y;
            knight.on_ground = 0;
        }
    } else if (knight.vy < 0) {
        // 向上移动（跳跃）
        if (check_ceiling_collision(knight.x, new_y)) {
            // 撞天花板：骑士头部精确贴住天花板
            int grid_y = (int)(new_y / TILE_SIZE);
            knight.y = (float)((grid_y + 1) * TILE_SIZE);
            knight.vy = 0;
            knight.on_ground = 0;
            
        } else {
            knight.y = new_y;
            knight.on_ground = 0;
        }
    }

    // 检查是否到达通关方块（只触发一次）
    int knight_grid_x = (int)((knight.x + knight.width / 2) / TILE_SIZE);
    int knight_grid_y = (int)((knight.y + knight.height / 2) / TILE_SIZE);
    if (get_block_type(knight_grid_x, knight_grid_y) == BLOCK_GOAL && !game_won) {
        // 设置游戏状态为通关，显示通关菜单
        game_won = 1;
        set_game_state(GAME_STATE_GAME_OVER);
        play_sound(SOUND_COIN); // 播放通关音效
        printf("恭喜通关！你成功到达终点！\n");
    }

    // 检查是否获得二连跳能力
    if (get_block_type(knight_grid_x, knight_grid_y) == BLOCK_DOUBLE_JUMP) {
        if (collect_double_jump_block(knight_grid_x, knight_grid_y)) {
            knight_enable_double_jump();
            show_skill_hint("double_jump");
            play_sound(SOUND_POWER_UP); // 播放技能获得音效
        }
    }

    // 检查是否获得冲刺能力
    if (get_block_type(knight_grid_x, knight_grid_y) == BLOCK_DASH) {
        if (collect_dash_block(knight_grid_x, knight_grid_y)) {
            knight_enable_dash();
            show_skill_hint("dash");
            play_sound(SOUND_POWER_UP); // 播放技能获得音效
        }
    }

    // 处理冲刺状态
    if (knight.is_dashing) {
        knight.dash_timer -= 1.0f / 60.0f;
        if (knight.dash_timer <= 0) {
            knight.is_dashing = 0;
            knight.dash_timer = 0.0f;
            knight.dash_cooldown = 0.5f; // 0.5秒冷却
        }
    }

    // 每帧递减冷却
    if (knight.dash_cooldown > 0) {
        knight.dash_cooldown -= 1.0f / 60.0f;
        if (knight.dash_cooldown < 0) knight.dash_cooldown = 0;
    }

    // 检查是否到达存档点方块
    int current_on_save = (get_block_type(knight_grid_x, knight_grid_y) == BLOCK_SAVE);
    if (current_on_save && !on_save_block) {
        // 刚刚进入存档点，进行保存
        save_x = knight.x;
        save_y = knight.y;
        save_set = 1;
        printf("存档点已记录：(%f, %f)\n", save_x, save_y);
    }
    on_save_block = current_on_save; // 更新存档点状态
    // 检查是否到达陷阱方块
    if (get_block_type(knight_grid_x, knight_grid_y) == BLOCK_TRAP) {
        if (knight.lives > 1) {
            knight_take_damage();
            knight.x = save_x;
            knight.y = save_y;
            knight.vx = 0;
            knight.vy = 0;
            printf("骑士踩到陷阱，扣血并回到存档点！\n");
        } else {
            knight_take_damage();
            printf("骑士踩到陷阱，死亡！\n");
        }
    }

    // 检查是否到达镜头移动方块
    extern float camera_offset_x;
    if (get_block_type(knight_grid_x, knight_grid_y) == BLOCK_CAMERA_MOVE) {
        camera_offset_x = 15;
    } else {
        camera_offset_x = 0;
    }
}

// 骑士跳跃
void knight_jump() {
    if (knight.on_ground && knight.alive) {
        knight.vy = JUMP_FORCE;
        knight.on_ground = 0;
        knight.double_jump_used = 0;
        play_sound(SOUND_JUMP); // 播放跳跃音效
    } else if (knight.can_double_jump && !knight.double_jump_used && knight.alive) {
        knight.vy = JUMP_FORCE;
        knight.double_jump_used = 1;
        play_sound(SOUND_JUMP); // 播放跳跃音效
    }
}

// 设置骑士目标速度
void set_knight_target_velocity(float target_vx) {
    knight.target_vx = target_vx;
}

// 获取骑士位置
void get_knight_position(float* x, float* y) {
    *x = knight.x;
    *y = knight.y;
}

// 获取骑士尺寸
void get_knight_size(int* w, int* h) {
    *w = knight.width;
    *h = knight.height;
}

// 骑士受伤
void knight_take_damage() {
    if (!knight.alive || knight.hurt_timer > 0 || knight.is_taking_damage || knight.is_dying) return;
    knight.lives--;
    printf("骑士受伤！剩余生命：%d\n", knight.lives);
    play_sound(SOUND_HURT); // 播放受伤音效

    if (knight.lives <= 0) {
        knight.is_dying = 1;
        knight.state_timer = 1.2f;
        printf("骑士死亡！游戏结束！\n");
    } else {
        knight.is_taking_damage = 1;
        knight.state_timer = 0.6f;
        knight.hurt_timer = 2.0f;
        knight.vx *= 0.3f;
        knight.target_vx = 0;
        // 不再回到起点
    }
}

// 检查骑士是否处于无敌状态
int knight_is_invulnerable() {
    return knight.hurt_timer > 0;
}

// 获取骑士生命数
int knight_get_lives() {
    return knight.lives;
}

// 动画相关接口实现
KnightAnimationState get_knight_animation_state() {
    return knight.anim_state;
}

int get_knight_animation_frame() {
    return knight.anim_frame;
}

int is_knight_facing_right() {
    return knight.facing_right;
}

// 获得二连跳能力
void knight_enable_double_jump() {
    knight.can_double_jump = 1;
}

// 冲刺逻辑
void knight_dash() {
    if (knight.can_dash && !knight.is_dashing && knight.alive && knight.dash_cooldown <= 0.0f) {
        knight.is_dashing = 1;
        knight.dash_timer = 0.18f; // 冲刺持续0.18秒
        knight.vx = knight.facing_right ? DASH_SPEED : -DASH_SPEED;
    }
}

// 获得冲刺能力
void knight_enable_dash() {
    knight.can_dash = 1;
} 
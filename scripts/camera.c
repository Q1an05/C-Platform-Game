// camera.c
// 摄像机系统实现

#include "camera.h"
#include "map.h"
#include <math.h>

// 全局摄像机对象
Camera camera;

// 摄像机参数
#define TILE_SIZE 32
#define CAMERA_FOLLOW_SPEED 0.12f  // 普通跟随速度
#define CAMERA_DASH_FOLLOW_SPEED 0.65f  // 冲刺时跟随速度（大幅提升）
#define CAMERA_DEAD_ZONE 48.0f     // 普通死区
#define CAMERA_DASH_DEAD_ZONE 24.0f // 冲刺时死区（减小一半）
#define CAMERA_VERTICAL_DEAD_ZONE 126.0f   // 竖直方向死区更大
#define CAMERA_VERTICAL_SPEED_SCALE 0.15f // 竖直方向跟随速度更慢

// 预测性跟随参数
#define CAMERA_PREDICTION_FRAMES 8  // 预测8帧后的位置
#define CAMERA_MAX_PREDICTION_DISTANCE 80.0f // 最大预测距离

float camera_offset_x = 0.0f;

// 初始化摄像机
void init_camera(int screen_width, int screen_height) {
    camera.x = 0.0f;
    camera.y = 0.0f;
    camera.screen_width = screen_width;
    camera.screen_height = screen_height;
    camera.follow_speed = CAMERA_FOLLOW_SPEED;
    camera.dash_follow_speed = CAMERA_DASH_FOLLOW_SPEED;
    camera.dead_zone_width = CAMERA_DEAD_ZONE;
    camera.target_x = 0.0f;
    camera.target_y = 0.0f;
    camera.velocity_x = 0.0f;
    camera.velocity_y = 0.0f;
}

// 优化的摄像机更新函数（考虑角色状态）
void update_camera_with_state(float target_x, float target_y, float target_vx, int is_dashing, int facing_right) {
    // 根据冲刺状态选择参数
    float current_follow_speed = is_dashing ? camera.dash_follow_speed : camera.follow_speed;
    float current_dead_zone = is_dashing ? CAMERA_DASH_DEAD_ZONE : camera.dead_zone_width;
    float max_follow_speed = is_dashing ? 0.8f : 0.3f; // 冲刺时允许更高的最大速度
    
    // 预测性跟随：根据角色速度预测未来位置
    float predicted_x = target_x;
    if (is_dashing && fabsf(target_vx) > 0.1f) {
        // 计算预测位置（但限制最大距离）
        float prediction_distance = target_vx * CAMERA_PREDICTION_FRAMES / 60.0f;
        if (fabsf(prediction_distance) > CAMERA_MAX_PREDICTION_DISTANCE) {
            prediction_distance = facing_right ? CAMERA_MAX_PREDICTION_DISTANCE : -CAMERA_MAX_PREDICTION_DISTANCE;
        }
        predicted_x = target_x + prediction_distance;
    }
    
    // 计算摄像机应该跟随的目标位置（让骑士在屏幕中央偏左，垂直方向往上抬一格）
    float target_camera_x = predicted_x - camera.screen_width / 3.0f;
    float target_camera_y = target_y - camera.screen_height / 2.0f - TILE_SIZE;
    
    // 死区检测参数
    float screen_knight_x = target_x - camera.x;
    float dead_zone_left = camera.screen_width / 2.0f - current_dead_zone / 2.0f;
    float dead_zone_right = camera.screen_width / 2.0f + current_dead_zone / 2.0f;
    
    float screen_knight_y = target_y - camera.y;
    float dead_zone_top = camera.screen_height / 2.0f - CAMERA_VERTICAL_DEAD_ZONE / 2.0f;
    float dead_zone_bottom = camera.screen_height / 2.0f + CAMERA_VERTICAL_DEAD_ZONE / 2.0f;
    
    // 水平方向跟随 - 优化的动态跟随速度
    if (screen_knight_x < dead_zone_left || screen_knight_x > dead_zone_right || is_dashing) {
        float dx = target_camera_x - camera.x;
        
        // 冲刺时使用更激进的跟随策略
        if (is_dashing) {
            // 冲刺时：立即跟随 + 预测性移动
            float distance_factor = fabsf(dx) / 50.0f + 1.0f;
            float dynamic_speed = current_follow_speed * distance_factor;
            if (dynamic_speed > max_follow_speed) dynamic_speed = max_follow_speed;
            
            // 如果角色冲刺方向与摄像机需要移动方向一致，进一步加速
            if ((target_vx > 0 && dx > 0) || (target_vx < 0 && dx < 0)) {
                dynamic_speed *= 1.3f; // 额外30%加速
                if (dynamic_speed > 0.9f) dynamic_speed = 0.9f; // 但不超过90%
            }
            
            camera.x += dx * dynamic_speed;
        } else {
            // 普通移动：使用原有逻辑
            float distance_factor = fabsf(dx) / 100.0f + 1.0f;
            float dynamic_speed = current_follow_speed * distance_factor;
            if (dynamic_speed > max_follow_speed) dynamic_speed = max_follow_speed;
            camera.x += dx * dynamic_speed;
        }
    }
    
    // 竖直方向跟随 - 死区更大，速度更慢（不受冲刺影响）
    if (screen_knight_y < dead_zone_top || screen_knight_y > dead_zone_bottom) {
        float dy = target_camera_y - camera.y;
        float distance_factor_y = fabsf(dy) / 100.0f + 1.0f;
        float dynamic_speed_y = camera.follow_speed * CAMERA_VERTICAL_SPEED_SCALE * distance_factor_y;
        if (dynamic_speed_y > 0.15f) dynamic_speed_y = 0.15f;
        camera.y += dy * dynamic_speed_y;
    }
    
    // 限制摄像机边界（不能超出地图范围）
    float max_camera_x = (MAP_WIDTH * TILE_SIZE) - camera.screen_width;
    if (camera.x < 0) camera.x = 0;
    if (camera.x > max_camera_x) camera.x = max_camera_x;
    
    float max_camera_y = (MAP_HEIGHT * TILE_SIZE) - camera.screen_height;
    if (max_camera_y < 0) max_camera_y = 0;
    if (camera.y < 0) camera.y = 0;
    if (camera.y > max_camera_y) camera.y = max_camera_y;

    // 镜头偏移（如镜头移动方块）
    camera.x += camera_offset_x;
}

// 更新摄像机位置（保持向后兼容的原函数）
void update_camera(float target_x, float target_y) {
    // 调用优化版本，使用默认参数（非冲刺状态）
    update_camera_with_state(target_x, target_y, 0.0f, 0, 1);
}

// 获取摄像机偏移量
void get_camera_offset(float* offset_x, float* offset_y) {
    if (offset_x) *offset_x = camera.x;
    if (offset_y) *offset_y = camera.y;
}

// 检查对象是否在摄像机视野内
int is_in_camera_view(float world_x, float world_y, int width, int height) {
    float screen_x, screen_y;
    world_to_screen(world_x, world_y, &screen_x, &screen_y);
    
    // 检查对象是否与屏幕矩形相交
    return (screen_x + width > 0 && 
            screen_x < camera.screen_width &&
            screen_y + height > 0 && 
            screen_y < camera.screen_height);
}

// 世界坐标转屏幕坐标
void world_to_screen(float world_x, float world_y, float* screen_x, float* screen_y) {
    if (screen_x) *screen_x = world_x - camera.x;
    if (screen_y) *screen_y = world_y - camera.y;
}

// 屏幕坐标转世界坐标
void screen_to_world(float screen_x, float screen_y, float* world_x, float* world_y) {
    if (world_x) *world_x = screen_x + camera.x;
    if (world_y) *world_y = screen_y + camera.y;
} 
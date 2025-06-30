// camera.c
// 摄像机系统实现

#include "camera.h"
#include "map.h"
#include <math.h>

// 全局摄像机对象
Camera camera;

// 摄像机参数
#define TILE_SIZE 32
#define CAMERA_FOLLOW_SPEED 0.1f
#define CAMERA_DEAD_ZONE 64.0f

// 初始化摄像机
void init_camera(int screen_width, int screen_height) {
    camera.x = 0.0f;
    camera.y = 0.0f;
    camera.screen_width = screen_width;
    camera.screen_height = screen_height;
    camera.follow_speed = CAMERA_FOLLOW_SPEED;
    camera.dead_zone_width = CAMERA_DEAD_ZONE;
}

// 更新摄像机位置（跟随目标）
void update_camera(float target_x, float target_y) {
    // 计算摄像机应该跟随的目标位置（让马里奥在屏幕中央偏左）
    float target_camera_x = target_x - camera.screen_width / 3.0f;
    float target_camera_y = target_y - camera.screen_height / 2.0f;
    
    // 死区检测（只有马里奥离开死区时摄像机才移动）
    float screen_mario_x = target_x - camera.x;
    float dead_zone_left = camera.screen_width / 2.0f - camera.dead_zone_width / 2.0f;
    float dead_zone_right = camera.screen_width / 2.0f + camera.dead_zone_width / 2.0f;
    
    // 水平方向跟随
    if (screen_mario_x < dead_zone_left || screen_mario_x > dead_zone_right) {
        float dx = target_camera_x - camera.x;
        camera.x += dx * camera.follow_speed;
    }
    
    // 垂直方向跟随（可选，通常平台游戏只水平跟随）
    float dy = target_camera_y - camera.y;
    camera.y += dy * camera.follow_speed * 0.5f; // 垂直跟随较慢
    
    // 限制摄像机边界（不能超出地图范围）
    float max_camera_x = (MAP_WIDTH * TILE_SIZE) - camera.screen_width;
    if (camera.x < 0) camera.x = 0;
    if (camera.x > max_camera_x) camera.x = max_camera_x;
    
    float max_camera_y = (MAP_HEIGHT * TILE_SIZE) - camera.screen_height;
    if (camera.y < 0) camera.y = 0;
    if (camera.y > max_camera_y) camera.y = max_camera_y;
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
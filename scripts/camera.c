// camera.c
// 摄像机系统实现

#include "camera.h"
#include "map.h"
#include <math.h>

// 全局摄像机对象
Camera camera;

// 摄像机参数
#define TILE_SIZE 32
#define CAMERA_FOLLOW_SPEED 0.12f  // 稍微提高跟随速度
#define CAMERA_DEAD_ZONE 48.0f     // 稍微减小死区

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
    
    // 水平方向跟随 - 使用动态跟随速度
    if (screen_mario_x < dead_zone_left || screen_mario_x > dead_zone_right) {
        float dx = target_camera_x - camera.x;
        // 距离越远，跟随速度越快（动态跟随）
        float distance_factor = fabsf(dx) / 100.0f + 1.0f;
        float dynamic_speed = camera.follow_speed * distance_factor;
        if (dynamic_speed > 0.3f) dynamic_speed = 0.3f; // 限制最大跟随速度
        
        camera.x += dx * dynamic_speed;
    }
    
    // 垂直方向跟随限制（减少垂直摄像机移动）
    float dy = target_camera_y - camera.y;
    if (fabsf(dy) > 32.0f) { // 只有垂直距离超过一个瓦片时才跟随
        camera.y += dy * camera.follow_speed * 0.3f; // 垂直跟随更慢
    }
    
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
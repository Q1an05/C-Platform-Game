// camera.h
// 摄像机系统头文件，实现卷轴视角移动

#ifndef CAMERA_H
#define CAMERA_H

// 摄像机结构体
typedef struct {
    float x, y;              // 摄像机在世界中的位置
    int screen_width;        // 屏幕宽度
    int screen_height;       // 屏幕高度
    float follow_speed;      // 跟随速度（0.0-1.0，1.0为立即跟随）
    float dead_zone_width;   // 死区宽度（马里奥在这个范围内摄像机不移动）
} Camera;

// 全局摄像机对象
extern Camera camera;

// 全局摄像机额外偏移变量
extern float camera_offset_x;

// 摄像机相关函数接口
void init_camera(int screen_width, int screen_height);  // 初始化摄像机
void update_camera(float target_x, float target_y);     // 更新摄像机位置
void get_camera_offset(float* offset_x, float* offset_y); // 获取摄像机偏移
int is_in_camera_view(float world_x, float world_y, int width, int height); // 检查对象是否在视野内

// 坐标转换函数
void world_to_screen(float world_x, float world_y, float* screen_x, float* screen_y);
void screen_to_world(float screen_x, float screen_y, float* world_x, float* world_y);

#endif // CAMERA_H 
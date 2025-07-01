// blocks.h
// 奖励方块系统头文件

#ifndef BLOCKS_H
#define BLOCKS_H

// 方块类型枚举
typedef enum {
    BLOCK_NONE,     // 无方块
    BLOCK_NORMAL,   // 普通砖块
    BLOCK_GOAL,     // 通关方块
    BLOCK_GRASS,    // 草地方块（地表）
    BLOCK_MUD,      // 泥土方块（地下）
    BLOCK_ENEMY_BARRIER,  // 敌人屏障（不可见，只阻挡敌人）
    BLOCK_DOUBLE_JUMP,     // 二连跳奖励方块
    BLOCK_DASH,             // 冲刺奖励方块
    BLOCK_TRAP,             // 陷阱方块（透明，不可见，角色触碰扣血回到起点）
    BLOCK_SAVE,             // 存档点方块（透明，不可见，角色触碰后记录当前位置）
    BLOCK_CAMERA_MOVE        // 镜头移动方块（不可见，角色碰撞时触发摄像机移动）
} BlockType;

// 方块结构体
typedef struct {
    int x, y;               // 在地图中的位置（瓦片坐标）
    BlockType type;         // 方块类型
} Block;

// 方块系统接口
void update_blocks();                            // 更新方块状态
BlockType get_block_type(int map_x, int map_y);  // 获取指定位置的方块类型
void render_blocks();                             // 渲染方块（由render模块调用）
int collect_double_jump_block(int map_x, int map_y); // 收集二连跳奖励方块
int collect_dash_block(int map_x, int map_y); // 收集冲刺奖励方块

#endif // BLOCKS_H 
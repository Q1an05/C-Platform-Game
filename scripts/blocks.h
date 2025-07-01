// blocks.h
// 奖励方块系统头文件

#ifndef BLOCKS_H
#define BLOCKS_H

// 方块类型枚举
typedef enum {
    BLOCK_NONE,     // 无方块
    BLOCK_NORMAL,   // 普通砖块
    BLOCK_REWARD,   // 奖励方块（问号方块）
    BLOCK_GOAL,     // 通关方块
    BLOCK_GRASS,    // 草地方块（地表）
    BLOCK_MUD,      // 泥土方块（地下）
    BLOCK_ENEMY_BARRIER  // 敌人屏障（不可见，只阻挡敌人）
} BlockType;

// 方块状态枚举
typedef enum {
    BLOCK_STATE_NORMAL,   // 正常状态
    BLOCK_STATE_HIT       // 被撞击后（奖励已取出）
} BlockState;

// 方块结构体
typedef struct {
    int x, y;               // 在地图中的位置（瓦片坐标）
    BlockType type;         // 方块类型
    BlockState state;       // 方块状态
} Block;

// 方块系统接口
void init_blocks();                              // 初始化方块系统
void update_blocks();                            // 更新方块状态
BlockType get_block_type(int map_x, int map_y);  // 获取指定位置的方块类型
int hit_block(int map_x, int map_y);             // 撞击方块，返回是否成功
int is_block_hit(int map_x, int map_y);          // 检查方块是否已被撞击
void render_blocks();                             // 渲染方块（由render模块调用）

#endif // BLOCKS_H 
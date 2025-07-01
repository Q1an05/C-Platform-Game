// blocks.c
// 奖励方块系统实现

#include "blocks.h"
#include "map.h"
#include <stdio.h>

// 方块状态数组，记录每个奖励方块是否被撞击
// 使用地图坐标作为索引，值为0表示正常，1表示被撞击
static int block_states[MAP_HEIGHT][MAP_WIDTH];

// 初始化方块系统
void init_blocks() {
    // 清空所有方块状态
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            block_states[y][x] = 0;  // 0 = 正常状态
        }
    }
    
    printf("方块系统初始化完成，地图中的'?'表示奖励方块\n");
}

// 更新方块状态
void update_blocks() {
    // 目前方块是静态的，不需要特殊更新
    // 未来可以在这里添加动画效果
}

// 获取指定位置的方块类型
BlockType get_block_type(int map_x, int map_y) {
    // 检查地图边界
    if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
        return BLOCK_NONE;
    }
    
    // 检查地图数据
    char tile = game_map[map_y][map_x];
    
    if (tile == '?') {
        return BLOCK_REWARD;  // 奖励方块
    } else if (tile == 'G') {
        return BLOCK_GRASS;  // 草地方块
    } else if (tile == 'M') {
        return BLOCK_MUD;    // 泥土方块
    } else if (tile == '#') {
        return BLOCK_NORMAL; // 普通砖块（可选）
    } else if (tile == 'T') {
        return BLOCK_GOAL;   // 通关方块
    } else if (tile == 'B') {
        return BLOCK_ENEMY_BARRIER;  // 敌人屏障（不可见）
    }
    
    return BLOCK_NONE;  // 空地
}

// 撞击方块
int hit_block(int map_x, int map_y) {
    // 检查边界
    if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
        return 0;
    }
    
    // 检查是否是奖励方块且未被撞击
    if (game_map[map_y][map_x] == '?' && block_states[map_y][map_x] == 0) {
        // 标记方块为已撞击
        block_states[map_y][map_x] = 1;
        
        printf("马里奥撞击奖励方块！获得蘑菇，变大变快！\n");
        return 1; // 撞击成功
    }
    
    return 0; // 没有撞击到奖励方块或已被撞击
}

// 检查方块是否已被撞击
int is_block_hit(int map_x, int map_y) {
    // 检查边界
    if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
        return 0;
    }
    
    // 只有奖励方块才需要检查撞击状态
    if (game_map[map_y][map_x] == '?') {
        return block_states[map_y][map_x];
    }
    
    return 0;
} 
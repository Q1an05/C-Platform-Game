// blocks.c
// 奖励方块系统实现

#include "blocks.h"
#include "map.h"
#include <stdio.h>

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
    
    if (tile == 'G') {
        return BLOCK_GRASS;  // 草地方块
    } else if (tile == 'M') {
        return BLOCK_MUD;    // 泥土方块
    } else if (tile == '#') {
        return BLOCK_NORMAL; // 普通砖块
    } else if (tile == 't') {
        return BLOCK_GOAL;   // 通关方块
    } else if (tile == 'B') {
        return BLOCK_ENEMY_BARRIER;  // 敌人屏障
    } else if (tile == 'F') {
        return BLOCK_DOUBLE_JUMP;    // 二连跳奖励方块
    } else if (tile == 'D') {
        return BLOCK_DASH;           // 冲刺奖励方块
    } else if (tile == 'T') {
        return BLOCK_TRAP;          // 陷阱方块
    } else if (tile == 'S') {
        return BLOCK_SAVE;          // 存档点方块
    } else if (tile == 'C') {
        return BLOCK_CAMERA_MOVE;    // 镜头移动方块
    }
    
    return BLOCK_NONE;  // 空地
}

// 二连跳奖励方块消失机制
int collect_double_jump_block(int map_x, int map_y) {
    if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
        return 0;
    }
    if (game_map[map_y][map_x] == 'F') {
        game_map[map_y][map_x] = ' '; // 方块消失
        return 1;
    }
    return 0;
}

// 冲刺奖励方块消失机制
int collect_dash_block(int map_x, int map_y) {
    if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
        return 0;
    }
    if (game_map[map_y][map_x] == 'D') {
        game_map[map_y][map_x] = ' ';
        return 1;
    }
    return 0;
} 
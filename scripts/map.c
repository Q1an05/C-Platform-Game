// map.c
// 地图数据实现

#include "map.h"
#include <string.h>

// 原始地图模板（不会被修改）
static const char original_map[MAP_HEIGHT][MAP_WIDTH+1] = {
    "                                                                                           ",
    "                                                                                           ",
    "                                                                                           ",
    "                                    B  E  B  D                                             ",
    "                                MMMMMMMMMMMMMMMMMMM                                        ",
    "                               M                                                           ",
    "                              M                                                            ",
    "                             M                                                             ",
    "                            M                                                              ",
    "                                             M                                             ",
    "                                         M   M                                             ",
    "                                     M   M   M                                             ",
    "              B    E    B        M   M   M   M      FSS     SSSCCCC                    t   ",
    "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG   GGGGGGGGG            GGGGGGGGGGGGG",
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMTTTMMMMMMMMMTTTTTTTTTTTTMMMMMMMMMMMMM"
};

// 当前游戏地图（可以被修改）
char game_map[MAP_HEIGHT][MAP_WIDTH+1] = {
    "                                                                                           ",
    "                                                                                           ",
    "                                                                                           ",
    "                                    B  E  B  D                                             ",
    "                                MMMMMMMMMMMMMMMMMMM                                        ",
    "                               M                                                           ",
    "                              M                                                            ",
    "                             M                                                             ",
    "                            M                                                              ",
    "                                             M                                             ",
    "                                         M   M                                             ",
    "                                     M   M   M                                             ",
    "              B    E    B        M   M   M   M      FSS     SSSCCCC                    t   ",
    "GGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG   GGGGGGGGG            GGGGGGGGGGGGG",
    "MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMTTTMMMMMMMMMTTTTTTTTTTTTMMMMMMMMMMMMM"
};

// 重置地图到初始状态
void reset_map() {
    // 从原始地图模板复制数据到游戏地图
    for (int y = 0; y < MAP_HEIGHT; y++) {
        strcpy(game_map[y], original_map[y]);
    }
} 
// map.h
// 地图相关头文件，定义地图数据和接口

#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH 80
#define MAP_HEIGHT 8

// 地图数组声明（每行+1用于字符串结尾）
extern char game_map[MAP_HEIGHT][MAP_WIDTH+1];

#endif // MAP_H 
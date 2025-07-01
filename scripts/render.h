// render.h
// 渲染相关头文件，声明SDL2渲染接口

#ifndef RENDER_H
#define RENDER_H

#include <SDL.h>
#include <SDL_image.h>
#include "mario.h"

// 初始化SDL2窗口和渲染器
int init_render();
// 渲染游戏画面
void render_game();
// 释放SDL2资源
void cleanup_render();

// 纹理管理函数
int load_textures();           // 加载所有纹理
void cleanup_textures();      // 清理纹理资源
SDL_Texture* get_grass_texture();   // 获取草地纹理
SDL_Texture* get_mud_texture();     // 获取泥土纹理

// 角色动画纹理管理函数
int load_player_animations();                              // 加载玩家动画纹理
void cleanup_player_animations();                          // 清理玩家动画纹理
SDL_Texture* get_player_animation_texture(MarioAnimationState state, int frame); // 获取指定动画帧纹理

// 提供全局访问的窗口和渲染器指针（可选）
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

#endif // RENDER_H 
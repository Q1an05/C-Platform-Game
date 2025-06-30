// render.h
// 渲染相关头文件，声明SDL2渲染接口

#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>

// 初始化SDL2窗口和渲染器
int init_render();
// 渲染游戏画面
void render_game();
// 释放SDL2资源
void cleanup_render();

// 提供全局访问的窗口和渲染器指针（可选）
extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;

#endif // RENDER_H 
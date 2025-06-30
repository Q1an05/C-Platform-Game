// sprite.c
// 素材管理系统实现

#include "sprite.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 全局精灵数组
Sprite sprites[SPRITE_COUNT];

// 素材文件名映射表
static const char* sprite_filenames[SPRITE_COUNT] = {
    "assets/sprites/mario_small_idle.png",      // SPRITE_MARIO_SMALL_IDLE
    "assets/sprites/mario_small_walk1.png",     // SPRITE_MARIO_SMALL_WALK1
    "assets/sprites/mario_small_walk2.png",     // SPRITE_MARIO_SMALL_WALK2
    "assets/sprites/mario_small_jump.png",      // SPRITE_MARIO_SMALL_JUMP
    "assets/sprites/mario_big_idle.png",        // SPRITE_MARIO_BIG_IDLE
    "assets/sprites/mario_big_walk1.png",       // SPRITE_MARIO_BIG_WALK1
    "assets/sprites/mario_big_walk2.png",       // SPRITE_MARIO_BIG_WALK2
    "assets/sprites/mario_big_jump.png",        // SPRITE_MARIO_BIG_JUMP
    "assets/sprites/goomba_walk1.png",          // SPRITE_GOOMBA_WALK1
    "assets/sprites/goomba_walk2.png",          // SPRITE_GOOMBA_WALK2
    "assets/sprites/goomba_dead.png",           // SPRITE_GOOMBA_DEAD
    "assets/sprites/block_brick.png",           // SPRITE_BLOCK_BRICK
    "assets/sprites/block_question.png",        // SPRITE_BLOCK_QUESTION
    "assets/sprites/block_used.png",            // SPRITE_BLOCK_USED
    "assets/sprites/background.png"             // SPRITE_BACKGROUND
};

// 初始化素材系统
int init_sprite_system(SDL_Renderer* renderer) {
    // 初始化SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image初始化失败！SDL_image Error: %s\n", IMG_GetError());
        return 0;
    }
    
    // 初始化所有精灵为未加载状态
    for (int i = 0; i < SPRITE_COUNT; i++) {
        sprites[i].texture = NULL;
        sprites[i].width = 0;
        sprites[i].height = 0;
        sprites[i].loaded = 0;
    }
    
    printf("素材系统初始化成功！\n");
    return 1;
}

// 清理素材系统
void cleanup_sprite_system() {
    // 释放所有已加载的纹理
    for (int i = 0; i < SPRITE_COUNT; i++) {
        if (sprites[i].texture) {
            SDL_DestroyTexture(sprites[i].texture);
            sprites[i].texture = NULL;
            sprites[i].loaded = 0;
        }
    }
    
    // 退出SDL_image
    IMG_Quit();
    printf("素材系统已清理！\n");
}

// 加载单个精灵
int load_sprite(SpriteID id, const char* filename, SDL_Renderer* renderer) {
    if (id >= SPRITE_COUNT) {
        printf("错误：无效的精灵ID：%d\n", id);
        return 0;
    }
    
    // 如果已经加载过，先释放
    if (sprites[id].texture) {
        SDL_DestroyTexture(sprites[id].texture);
    }
    
    // 加载图片表面
    SDL_Surface* loadedSurface = IMG_Load(filename);
    if (!loadedSurface) {
        printf("警告：无法加载图片 %s！IMG_Error: %s\n", filename, IMG_GetError());
        // 不返回失败，允许游戏继续运行
        sprites[id].texture = NULL;
        sprites[id].loaded = 0;
        return 0;
    }
    
    // 创建纹理
    SDL_Texture* newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (!newTexture) {
        printf("警告：无法创建纹理 %s！SDL Error: %s\n", filename, SDL_GetError());
        SDL_FreeSurface(loadedSurface);
        sprites[id].texture = NULL;
        sprites[id].loaded = 0;
        return 0;
    }
    
    // 保存纹理信息
    sprites[id].texture = newTexture;
    sprites[id].width = loadedSurface->w;
    sprites[id].height = loadedSurface->h;
    sprites[id].loaded = 1;
    
    // 释放表面
    SDL_FreeSurface(loadedSurface);
    
    printf("成功加载精灵：%s (%dx%d)\n", filename, sprites[id].width, sprites[id].height);
    return 1;
}

// 加载所有精灵
int load_all_sprites(SDL_Renderer* renderer) {
    int loaded_count = 0;
    
    printf("开始加载所有素材...\n");
    
    for (int i = 0; i < SPRITE_COUNT; i++) {
        if (load_sprite(i, sprite_filenames[i], renderer)) {
            loaded_count++;
        }
    }
    
    printf("素材加载完成！成功加载 %d/%d 个素材\n", loaded_count, SPRITE_COUNT);
    
    // 即使没有加载任何素材也继续运行，使用颜色填充代替
    return 1;
}

// 绘制精灵
void draw_sprite(SDL_Renderer* renderer, SpriteID id, int x, int y) {
    if (id >= SPRITE_COUNT) return;
    
    if (sprites[id].loaded && sprites[id].texture) {
        SDL_Rect dstRect = {x, y, sprites[id].width, sprites[id].height};
        SDL_RenderCopy(renderer, sprites[id].texture, NULL, &dstRect);
    } else {
        // 如果没有素材，使用颜色填充作为备用
        draw_fallback_sprite(renderer, id, x, y, sprites[id].width, sprites[id].height);
    }
}

// 缩放绘制精灵
void draw_sprite_scaled(SDL_Renderer* renderer, SpriteID id, int x, int y, int w, int h) {
    if (id >= SPRITE_COUNT) return;
    
    if (sprites[id].loaded && sprites[id].texture) {
        SDL_Rect dstRect = {x, y, w, h};
        SDL_RenderCopy(renderer, sprites[id].texture, NULL, &dstRect);
    } else {
        // 如果没有素材，使用颜色填充作为备用
        draw_fallback_sprite(renderer, id, x, y, w, h);
    }
}

// 翻转绘制精灵
void draw_sprite_flipped(SDL_Renderer* renderer, SpriteID id, int x, int y, SDL_RendererFlip flip) {
    if (id >= SPRITE_COUNT) return;
    
    if (sprites[id].loaded && sprites[id].texture) {
        SDL_Rect dstRect = {x, y, sprites[id].width, sprites[id].height};
        SDL_RenderCopyEx(renderer, sprites[id].texture, NULL, &dstRect, 0.0, NULL, flip);
    } else {
        // 如果没有素材，使用颜色填充作为备用
        draw_fallback_sprite(renderer, id, x, y, sprites[id].width, sprites[id].height);
    }
}

// 备用绘制（当没有素材时使用颜色）
void draw_fallback_sprite(SDL_Renderer* renderer, SpriteID id, int x, int y, int w, int h) {
    // 如果宽高为0，使用默认尺寸
    if (w <= 0) w = 32;
    if (h <= 0) h = 32;
    
    // 根据精灵类型选择颜色
    switch (id) {
        case SPRITE_MARIO_SMALL_IDLE:
        case SPRITE_MARIO_SMALL_WALK1:
        case SPRITE_MARIO_SMALL_WALK2:
        case SPRITE_MARIO_SMALL_JUMP:
        case SPRITE_MARIO_BIG_IDLE:
        case SPRITE_MARIO_BIG_WALK1:
        case SPRITE_MARIO_BIG_WALK2:
        case SPRITE_MARIO_BIG_JUMP:
            draw_colored_rect(renderer, x, y, w, h, 255, 0, 0, 255); // 红色
            break;
        case SPRITE_GOOMBA_WALK1:
        case SPRITE_GOOMBA_WALK2:
            draw_colored_rect(renderer, x, y, w, h, 101, 67, 33, 255); // 棕色
            break;
        case SPRITE_GOOMBA_DEAD:
            draw_colored_rect(renderer, x, y, w, h, 80, 80, 80, 255); // 灰色
            break;
        case SPRITE_BLOCK_BRICK:
            draw_colored_rect(renderer, x, y, w, h, 139, 69, 19, 255); // 砖色
            break;
        case SPRITE_BLOCK_QUESTION:
            draw_colored_rect(renderer, x, y, w, h, 255, 255, 0, 255); // 黄色
            break;
        case SPRITE_BLOCK_USED:
            draw_colored_rect(renderer, x, y, w, h, 160, 160, 160, 255); // 灰色
            break;
        default:
            draw_colored_rect(renderer, x, y, w, h, 128, 128, 128, 255); // 默认灰色
            break;
    }
}

// 绘制彩色矩形
void draw_colored_rect(SDL_Renderer* renderer, int x, int y, int w, int h, 
                       Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

// 检查精灵是否已加载
int is_sprite_loaded(SpriteID id) {
    if (id >= SPRITE_COUNT) return 0;
    return sprites[id].loaded;
}

// 创建动画
Animation* create_animation(SpriteID* frames, int frame_count, float frame_duration, int loop) {
    Animation* anim = malloc(sizeof(Animation));
    if (!anim) return NULL;
    
    anim->frames = malloc(sizeof(SpriteID) * frame_count);
    if (!anim->frames) {
        free(anim);
        return NULL;
    }
    
    // 复制帧数据
    for (int i = 0; i < frame_count; i++) {
        anim->frames[i] = frames[i];
    }
    
    anim->frame_count = frame_count;
    anim->frame_duration = frame_duration;
    anim->current_time = 0.0f;
    anim->current_frame = 0;
    anim->loop = loop;
    
    return anim;
}

// 更新动画
void update_animation(Animation* anim, float delta_time) {
    if (!anim || anim->frame_count <= 1) return;
    
    anim->current_time += delta_time;
    
    if (anim->current_time >= anim->frame_duration) {
        anim->current_time -= anim->frame_duration;
        anim->current_frame++;
        
        if (anim->current_frame >= anim->frame_count) {
            if (anim->loop) {
                anim->current_frame = 0;
            } else {
                anim->current_frame = anim->frame_count - 1;
            }
        }
    }
}

// 获取当前帧
SpriteID get_current_frame(Animation* anim) {
    if (!anim || anim->current_frame >= anim->frame_count) return SPRITE_MARIO_SMALL_IDLE;
    return anim->frames[anim->current_frame];
}

// 重置动画
void reset_animation(Animation* anim) {
    if (!anim) return;
    anim->current_time = 0.0f;
    anim->current_frame = 0;
}

// 销毁动画
void destroy_animation(Animation* anim) {
    if (!anim) return;
    if (anim->frames) free(anim->frames);
    free(anim);
} 
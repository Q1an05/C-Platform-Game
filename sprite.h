// sprite.h
// 素材管理系统头文件

#ifndef SPRITE_H
#define SPRITE_H

#include <SDL.h>

// 精灵ID枚举（用于识别不同的游戏素材）
typedef enum {
    SPRITE_MARIO_SMALL_IDLE,     // 小马里奥静止
    SPRITE_MARIO_SMALL_WALK1,    // 小马里奥走路帧1
    SPRITE_MARIO_SMALL_WALK2,    // 小马里奥走路帧2
    SPRITE_MARIO_SMALL_JUMP,     // 小马里奥跳跃
    SPRITE_MARIO_BIG_IDLE,       // 大马里奥静止
    SPRITE_MARIO_BIG_WALK1,      // 大马里奥走路帧1
    SPRITE_MARIO_BIG_WALK2,      // 大马里奥走路帧2
    SPRITE_MARIO_BIG_JUMP,       // 大马里奥跳跃
    SPRITE_GOOMBA_WALK1,         // 栗子小子走路帧1
    SPRITE_GOOMBA_WALK2,         // 栗子小子走路帧2
    SPRITE_GOOMBA_DEAD,          // 栗子小子死亡
    SPRITE_BLOCK_BRICK,          // 砖块
    SPRITE_BLOCK_QUESTION,       // 问号方块
    SPRITE_BLOCK_USED,           // 用过的方块
    SPRITE_BACKGROUND,           // 背景
    SPRITE_COUNT                 // 精灵总数
} SpriteID;

// 精灵结构体
typedef struct {
    SDL_Texture* texture;        // SDL纹理指针
    int width, height;           // 精灵尺寸
    int loaded;                  // 是否已加载
} Sprite;

// 动画结构体
typedef struct {
    SpriteID* frames;            // 动画帧数组
    int frame_count;             // 帧数
    float frame_duration;        // 每帧持续时间（秒）
    float current_time;          // 当前时间
    int current_frame;           // 当前帧索引
    int loop;                    // 是否循环
} Animation;

// 全局精灵数组
extern Sprite sprites[SPRITE_COUNT];

// 函数声明
int init_sprite_system(SDL_Renderer* renderer);     // 初始化素材系统
void cleanup_sprite_system();                       // 清理素材系统
int load_sprite(SpriteID id, const char* filename, SDL_Renderer* renderer); // 加载单个精灵
int load_all_sprites(SDL_Renderer* renderer);       // 加载所有精灵
void draw_sprite(SDL_Renderer* renderer, SpriteID id, int x, int y);         // 绘制精灵
void draw_sprite_scaled(SDL_Renderer* renderer, SpriteID id, int x, int y, int w, int h); // 缩放绘制
void draw_sprite_flipped(SDL_Renderer* renderer, SpriteID id, int x, int y, SDL_RendererFlip flip); // 翻转绘制

// 动画系统
Animation* create_animation(SpriteID* frames, int frame_count, float frame_duration, int loop);
void update_animation(Animation* anim, float delta_time);
SpriteID get_current_frame(Animation* anim);
void reset_animation(Animation* anim);
void destroy_animation(Animation* anim);

// 简单颜色填充（用于没有素材时的备用显示）
void draw_colored_rect(SDL_Renderer* renderer, int x, int y, int w, int h, 
                       Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// 备用绘制函数
void draw_fallback_sprite(SDL_Renderer* renderer, SpriteID id, int x, int y, int w, int h);

// 检查精灵是否已加载
int is_sprite_loaded(SpriteID id);

#endif // SPRITE_H 
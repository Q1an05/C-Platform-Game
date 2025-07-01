// render.c
// SDL2渲染模块实现，负责显示地图和马里奥

#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include "map.h"
#include "knight.h"
#include "camera.h"
#include "blocks.h"
#include "enemy.h"
#include "render.h"

// 全局窗口和渲染器指针
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

// 全局纹理指针
static SDL_Texture* grass_texture = NULL;
static SDL_Texture* mud_texture = NULL;
static SDL_Texture* fruit1_texture = NULL;
static SDL_Texture* fruit2_texture = NULL;
static SDL_Texture* fruit3_texture = NULL;

// 角色动画纹理数组
static SDL_Texture* player_idle_textures[4];   // idle动画4帧
static SDL_Texture* player_run_textures[16];   // run动画16帧
static SDL_Texture* player_hit_textures[4];    // hit动画4帧
static SDL_Texture* player_death_textures[4];  // death动画4帧

// 敌人动画纹理数组
static SDL_Texture* enemy_idle_textures[4];    // enemy idle动画4帧
static SDL_Texture* enemy_hit_textures[4];     // enemy hit动画4帧

// 每个格子的像素大小
#define TILE_SIZE 16

// 修改窗口和摄像机逻辑分辨率为16:9比例，缩小视野
#define CAMERA_VIEW_WIDTH 20
#define CAMERA_VIEW_HEIGHT 11

// 颜色定义
SDL_Color COLOR_BG       = {135, 206, 235, 255}; // 天空蓝
SDL_Color COLOR_WALL     = {139, 69, 19, 255};   // 棕色（砖块）
SDL_Color COLOR_KNIGHT   = {255, 0, 0, 255};     // 红色（骑士）
SDL_Color COLOR_REWARD   = {255, 255, 0, 255};   // 黄色（奖励方块）
SDL_Color COLOR_USED     = {160, 160, 160, 255}; // 灰色（用过的方块）
SDL_Color COLOR_ENEMY    = {101, 67, 33, 255};   // 棕色（敌人）
SDL_Color COLOR_ENEMY_DEAD = {80, 80, 80, 255};  // 深灰色（死亡敌人）
SDL_Color COLOR_GOAL     = {0, 220, 0, 255};   // 绿色（通关方块）

// 加载纹理的辅助函数
SDL_Texture* load_texture_from_file(const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        printf("无法加载图片 %s! SDL_image Error: %s\n", path, IMG_GetError());
        return NULL;
    }
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(gRenderer, surface);
    SDL_FreeSurface(surface);
    
    if (!texture) {
        printf("无法创建纹理 %s! SDL Error: %s\n", path, SDL_GetError());
        return NULL;
    }
    
    return texture;
}

// 加载所有纹理
int load_textures() {
    // 初始化SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        printf("SDL_image初始化失败! SDL_image Error: %s\n", IMG_GetError());
        return 0;
    }
    
    // 加载草地纹理
    grass_texture = load_texture_from_file("assets/sprites/world/grass.png");
    if (!grass_texture) {
        printf("草地纹理加载失败!\n");
        return 0;
    }
    
    // 加载泥土纹理
    mud_texture = load_texture_from_file("assets/sprites/world/mud.png");
    if (!mud_texture) {
        printf("泥土纹理加载失败!\n");
        return 0;
    }
    
    // 加载二连跳奖励方块纹理
    fruit1_texture = load_texture_from_file("assets/sprites/rewards/fruit1.png");
    if (!fruit1_texture) {
        printf("二连跳奖励方块纹理加载失败！\n");
        return 0;
    }
    
    // 加载冲刺奖励方块纹理
    fruit2_texture = load_texture_from_file("assets/sprites/rewards/fruit2.png");
    if (!fruit2_texture) {
        printf("冲刺奖励方块纹理加载失败！\n");
        return 0;
    }
    
    // 加载通关奖励方块纹理
    fruit3_texture = load_texture_from_file("assets/sprites/rewards/fruit3.png");
    if (!fruit3_texture) {
        printf("通关奖励方块纹理加载失败！\n");
        return 0;
    }
    
    printf("地图纹理加载成功！\n");
    return 1;
}

// 清理纹理资源
void cleanup_textures() {
    if (grass_texture) {
        SDL_DestroyTexture(grass_texture);
        grass_texture = NULL;
    }
    if (mud_texture) {
        SDL_DestroyTexture(mud_texture);
        mud_texture = NULL;
    }
    if (fruit1_texture) {
        SDL_DestroyTexture(fruit1_texture);
        fruit1_texture = NULL;
    }
    if (fruit2_texture) {
        SDL_DestroyTexture(fruit2_texture);
        fruit2_texture = NULL;
    }
    if (fruit3_texture) {
        SDL_DestroyTexture(fruit3_texture);
        fruit3_texture = NULL;
    }
    IMG_Quit();
}

// 获取草地纹理
SDL_Texture* get_grass_texture() {
    return grass_texture;
}

// 获取泥土纹理
SDL_Texture* get_mud_texture() {
    return mud_texture;
}

// 绘制彩色矩形的辅助函数
void draw_colored_rect(SDL_Renderer* renderer, int x, int y, int w, int h, SDL_Color color) {
    // 保存当前绘制颜色
    Uint8 old_r, old_g, old_b, old_a;
    SDL_GetRenderDrawColor(renderer, &old_r, &old_g, &old_b, &old_a);
    
    // 设置新颜色
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    // 绘制填充矩形
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
    
    // 恢复原来的颜色
    SDL_SetRenderDrawColor(renderer, old_r, old_g, old_b, old_a);
}

// 绘制纹理的辅助函数
void draw_texture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int w, int h) {
    if (!texture) return;
    
    SDL_Rect dest_rect = {x, y, w, h};
    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);
}

// 绘制可翻转纹理的辅助函数
void draw_texture_flip(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int w, int h, int flip_horizontal) {
    if (!texture) return;
    
    SDL_Rect dest_rect = {x, y, w, h};
    SDL_RendererFlip flip = flip_horizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, texture, NULL, &dest_rect, 0.0, NULL, flip);
}

// 初始化SDL2窗口和渲染器
int init_render() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init Error: %s", SDL_GetError());
        return 0;
    }
    
    // 16:9窗口，默认大小1600x900
    int window_width = 1600;
    int window_height = 900;
    
    gWindow = SDL_CreateWindow("超级骑士", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!gWindow) {
        SDL_Log("SDL_CreateWindow Error: %s", SDL_GetError());
        return 0;
    }
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!gRenderer) {
        SDL_Log("SDL_CreateRenderer Error: %s", SDL_GetError());
        return 0;
    }
    // 设置逻辑分辨率，保持固定比例缩放
    int logical_width = CAMERA_VIEW_WIDTH * TILE_SIZE;  // 320
    int logical_height = CAMERA_VIEW_HEIGHT * TILE_SIZE; // 176
    SDL_RenderSetLogicalSize(gRenderer, logical_width, logical_height);
    
    // 设置缩放质量为最佳
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    
    // 加载纹理
    if (!load_textures()) {
        printf("纹理加载失败！\n");
        return 0;
    }
    
    // 初始化摄像机（使用逻辑分辨率）
    init_camera(logical_width, logical_height);
    
    // 加载角色动画纹理
    if (!load_player_animations()) {
        printf("角色动画纹理加载失败！\n");
        return 0;
    }
    
    // 加载敌人动画纹理
    if (!load_enemy_animations()) {
        printf("敌人动画纹理加载失败！\n");
        return 0;
    }
    
    printf("渲染系统初始化成功！格子大小: %dx%d, 视野: %dx%d (16:9)\n", TILE_SIZE, TILE_SIZE, CAMERA_VIEW_WIDTH, CAMERA_VIEW_HEIGHT);
    return 1;
}

// 渲染游戏画面
void render_game() {
    // 清屏（天空蓝）
    SDL_SetRenderDrawColor(gRenderer, COLOR_BG.r, COLOR_BG.g, COLOR_BG.b, COLOR_BG.a);
    SDL_RenderClear(gRenderer);

    // 获取摄像机浮点数位置
    float camera_x_float, camera_y_float;
    get_camera_offset(&camera_x_float, &camera_y_float);
    
    // 使用浮点数偏移量，只在最终渲染位置时转换为整数
    float render_offset_x = camera_x_float;
    float render_offset_y = camera_y_float;

    // 绘制地图（只渲染视野内的部分）
    int start_x = (int)(camera_x_float / TILE_SIZE);
    int end_x = start_x + (32 * TILE_SIZE) / TILE_SIZE + 2;
    if (start_x < 0) start_x = 0;
    if (end_x > MAP_WIDTH) end_x = MAP_WIDTH;
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = start_x; x < end_x; x++) {
            // 计算屏幕坐标，使用整数坐标避免子像素渲染
            SDL_Rect rect = {
                (int)(x * TILE_SIZE - render_offset_x), 
                (int)(y * TILE_SIZE - render_offset_y), 
                TILE_SIZE, 
                TILE_SIZE
            };
            
            // 检查方块类型并绘制
            BlockType block_type = get_block_type(x, y);
            
            if (block_type == BLOCK_DOUBLE_JUMP) {
                draw_texture(gRenderer, fruit1_texture, rect.x, rect.y, rect.w, rect.h);
            } else if (block_type == BLOCK_GOAL) {
                draw_texture(gRenderer, fruit3_texture, rect.x, rect.y, rect.w, rect.h);
            } else if (block_type == BLOCK_GRASS) {
                // 草地方块：使用草地纹理
                draw_texture(gRenderer, grass_texture, rect.x, rect.y, rect.w, rect.h);
            } else if (block_type == BLOCK_MUD) {
                // 泥土方块：使用泥土纹理
                draw_texture(gRenderer, mud_texture, rect.x, rect.y, rect.w, rect.h);
            } else if (block_type == BLOCK_NORMAL || game_map[y][x] == '#') {
                // 普通砖块（备用，不应该到达这里）
                draw_colored_rect(gRenderer, rect.x, rect.y, rect.w, rect.h, COLOR_WALL);
            } else if (block_type == BLOCK_DASH) {
                draw_texture(gRenderer, fruit2_texture, rect.x, rect.y, rect.w, rect.h);
            }
        }
    }

    // 绘制敌人
    for (int i = 0; i < enemy_count; i++) {
        float enemy_x, enemy_y;
        int enemy_w, enemy_h;
        EnemyState enemy_state;
        
        get_enemy_info(i, &enemy_x, &enemy_y, &enemy_w, &enemy_h, &enemy_state);
        
        // 只渲染存活的或正在死亡动画的敌人
        if (enemy_state == ENEMY_STATE_DEAD) continue;
        
        // 计算屏幕坐标，使用平滑的浮点数计算
        SDL_Rect enemyRect = {
            (int)(enemy_x - render_offset_x),
            (int)(enemy_y - render_offset_y),
            enemy_w,
            enemy_h
        };
        
        // 根据敌人状态绘制
        if (enemy_state == ENEMY_STATE_STOMPED) {
            // 即使是被踩死状态，也要尝试使用动画纹理渲染
            EnemyAnimationState anim_state = get_enemy_animation_state(i);
            int anim_frame = get_enemy_animation_frame(i);
            int direction = get_enemy_direction(i);
            
            // 获取当前帧的纹理
            SDL_Texture* enemy_texture = get_enemy_animation_texture(anim_state, anim_frame);
            
            if (enemy_texture) {
                // 使用动画纹理渲染，支持水平翻转
                // direction为-1时翻转（面向左），为1时不翻转（面向右）
                draw_texture_flip(gRenderer, enemy_texture, 
                                 enemyRect.x, enemyRect.y, enemyRect.w, enemyRect.h, 
                                 direction == -1);
            } else {
                // 备用：如果纹理加载失败，使用灰色矩形表示死亡状态
                draw_colored_rect(gRenderer, enemyRect.x, enemyRect.y, enemyRect.w, enemyRect.h, COLOR_ENEMY_DEAD);
            }
        } else {
            // 获取敌人动画信息
            EnemyAnimationState anim_state = get_enemy_animation_state(i);
            int anim_frame = get_enemy_animation_frame(i);
            int direction = get_enemy_direction(i);
            
            // 获取当前帧的纹理
            SDL_Texture* enemy_texture = get_enemy_animation_texture(anim_state, anim_frame);
            
            if (enemy_texture) {
                // 使用动画纹理渲染，支持水平翻转
                // direction为-1时翻转（面向左），为1时不翻转（面向右）
                draw_texture_flip(gRenderer, enemy_texture, 
                                 enemyRect.x, enemyRect.y, enemyRect.w, enemyRect.h, 
                                 direction == -1);
            } else {
                // 备用：如果纹理加载失败，使用纯色矩形
                draw_colored_rect(gRenderer, enemyRect.x, enemyRect.y, enemyRect.w, enemyRect.h, COLOR_ENEMY);
            }
        }
    }

    // 绘制骑士（最后绘制，确保在前景）
    float knight_world_x, knight_world_y;
    int knight_w, knight_h;
    get_knight_position(&knight_world_x, &knight_world_y);
    get_knight_size(&knight_w, &knight_h);
    
    // 计算屏幕坐标，使用平滑的浮点数计算
    SDL_Rect knightRect = {
        (int)(knight_world_x - render_offset_x), 
        (int)(knight_world_y - render_offset_y), 
        knight_w, 
        knight_h
    };
    
    // 决定是否绘制骑士（受击时不闪烁，只有无敌且不在播放受击动画时才闪烁）
    int should_draw = 1;
    if (knight_is_invulnerable()) {
        // 获取骑士动画状态
        KnightAnimationState current_anim_state = get_knight_animation_state();
        
        // 只有在无敌状态下且不在播放受击或死亡动画时才闪烁
        if (current_anim_state != KNIGHT_ANIM_HIT && current_anim_state != KNIGHT_ANIM_DEATH) {
            // 使用简单的时间计算实现闪烁
            static int flash_counter = 0;
            flash_counter++;
            should_draw = (flash_counter / 6) % 2; // 每6帧切换一次
        }
    }
    
    if (should_draw) {
        // 获取当前动画状态和帧
        KnightAnimationState anim_state = get_knight_animation_state();
        int anim_frame = get_knight_animation_frame();
        int facing_right = is_knight_facing_right();
        
        // 获取当前帧的纹理
        SDL_Texture* knight_texture = get_player_animation_texture(anim_state, anim_frame);
        
        if (knight_texture) {
            // 使用动画纹理渲染，支持水平翻转
            // 注意：facing_right为0时翻转，为1时不翻转
            draw_texture_flip(gRenderer, knight_texture, 
                             knightRect.x, knightRect.y, knightRect.w, knightRect.h, 
                             !facing_right);
        } else {
            // 备用：如果纹理加载失败，使用纯色矩形
            draw_colored_rect(gRenderer, knightRect.x, knightRect.y, knightRect.w, knightRect.h, COLOR_KNIGHT);
        }
    }

    SDL_RenderPresent(gRenderer);
}

// 释放SDL2资源
void cleanup_render() {
    cleanup_textures();  // 清理纹理
    cleanup_player_animations();  // 清理角色动画纹理
    cleanup_enemy_animations();   // 清理敌人动画纹理
    if (gRenderer) SDL_DestroyRenderer(gRenderer);
    if (gWindow) SDL_DestroyWindow(gWindow);
    SDL_Quit();
    printf("渲染系统已清理！\n");
}

// 加载角色动画纹理
int load_player_animations() {
    char filename[256];
    
    // 加载idle动画帧
    for (int i = 0; i < 4; i++) {
        snprintf(filename, sizeof(filename), "assets/sprites/player/player_idle%d.png", i + 1);
        player_idle_textures[i] = load_texture_from_file(filename);
        if (!player_idle_textures[i]) {
            printf("无法加载idle动画帧 %d: %s\n", i + 1, filename);
            return 0;
        }
    }
    
    // 加载run动画帧
    for (int i = 0; i < 16; i++) {
        snprintf(filename, sizeof(filename), "assets/sprites/player/player_run%d.png", i + 1);
        player_run_textures[i] = load_texture_from_file(filename);
        if (!player_run_textures[i]) {
            printf("无法加载run动画帧 %d: %s\n", i + 1, filename);
            return 0;
        }
    }
    
    // 加载hit动画帧
    for (int i = 0; i < 4; i++) {
        snprintf(filename, sizeof(filename), "assets/sprites/player/player_hit%d.png", i + 1);
        player_hit_textures[i] = load_texture_from_file(filename);
        if (!player_hit_textures[i]) {
            printf("无法加载hit动画帧 %d: %s\n", i + 1, filename);
            return 0;
        }
    }
    
    // 加载death动画帧
    for (int i = 0; i < 4; i++) {
        snprintf(filename, sizeof(filename), "assets/sprites/player/player_death%d.png", i + 1);
        player_death_textures[i] = load_texture_from_file(filename);
        if (!player_death_textures[i]) {
            printf("无法加载death动画帧 %d: %s\n", i + 1, filename);
            return 0;
        }
    }
    
    printf("角色动画纹理加载成功！(idle: 4帧, run: 16帧, hit: 4帧, death: 4帧)\n");
    return 1;
}

// 清理角色动画纹理
void cleanup_player_animations() {
    // 清理idle动画纹理
    for (int i = 0; i < 4; i++) {
        if (player_idle_textures[i]) {
            SDL_DestroyTexture(player_idle_textures[i]);
            player_idle_textures[i] = NULL;
        }
    }
    
    // 清理run动画纹理
    for (int i = 0; i < 16; i++) {
        if (player_run_textures[i]) {
            SDL_DestroyTexture(player_run_textures[i]);
            player_run_textures[i] = NULL;
        }
    }
    
    // 清理hit动画纹理
    for (int i = 0; i < 4; i++) {
        if (player_hit_textures[i]) {
            SDL_DestroyTexture(player_hit_textures[i]);
            player_hit_textures[i] = NULL;
        }
    }
    
    // 清理death动画纹理
    for (int i = 0; i < 4; i++) {
        if (player_death_textures[i]) {
            SDL_DestroyTexture(player_death_textures[i]);
            player_death_textures[i] = NULL;
        }
    }
}

// 获取指定动画状态和帧的纹理
SDL_Texture* get_player_animation_texture(KnightAnimationState state, int frame) {
    if (state == KNIGHT_ANIM_IDLE) {
        if (frame >= 0 && frame < 4) {
            return player_idle_textures[frame];
        }
    } else if (state == KNIGHT_ANIM_RUN) {
        if (frame >= 0 && frame < 16) {
            return player_run_textures[frame];
        }
    } else if (state == KNIGHT_ANIM_HIT) {
        if (frame >= 0 && frame < 4) {
            return player_hit_textures[frame];
        }
    } else if (state == KNIGHT_ANIM_DEATH) {
        if (frame >= 0 && frame < 4) {
            return player_death_textures[frame];
        }
    }
    
    // 默认返回第一帧idle纹理
    return player_idle_textures[0];
}

// 加载敌人动画纹理
int load_enemy_animations() {
    char filename[256];
    
    // 加载enemy idle动画帧
    for (int i = 0; i < 4; i++) {
        snprintf(filename, sizeof(filename), "assets/sprites/enemy/enemy_idle%d.png", i + 1);
        enemy_idle_textures[i] = load_texture_from_file(filename);
        if (!enemy_idle_textures[i]) {
            printf("无法加载enemy idle动画帧 %d: %s\n", i + 1, filename);
            return 0;
        }
    }
    
    // 加载enemy hit动画帧
    for (int i = 0; i < 4; i++) {
        snprintf(filename, sizeof(filename), "assets/sprites/enemy/enemy_hit%d.png", i + 1);
        enemy_hit_textures[i] = load_texture_from_file(filename);
        if (!enemy_hit_textures[i]) {
            printf("无法加载enemy hit动画帧 %d: %s\n", i + 1, filename);
            return 0;
        }
    }
    
    printf("敌人动画纹理加载成功！(idle: 4帧, hit: 4帧)\n");
    return 1;
}

// 清理敌人动画纹理
void cleanup_enemy_animations() {
    // 清理enemy idle动画纹理
    for (int i = 0; i < 4; i++) {
        if (enemy_idle_textures[i]) {
            SDL_DestroyTexture(enemy_idle_textures[i]);
            enemy_idle_textures[i] = NULL;
        }
    }
    
    // 清理enemy hit动画纹理
    for (int i = 0; i < 4; i++) {
        if (enemy_hit_textures[i]) {
            SDL_DestroyTexture(enemy_hit_textures[i]);
            enemy_hit_textures[i] = NULL;
        }
    }
}

// 获取指定敌人动画状态和帧的纹理
SDL_Texture* get_enemy_animation_texture(EnemyAnimationState state, int frame) {
    if (state == ENEMY_ANIM_IDLE) {
        if (frame >= 0 && frame < 4) {
            return enemy_idle_textures[frame];
        }
    } else if (state == ENEMY_ANIM_HIT) {
        if (frame >= 0 && frame < 4) {
            return enemy_hit_textures[frame];
        }
    }
    
    // 默认返回第一帧idle纹理
    return enemy_idle_textures[0];
} 
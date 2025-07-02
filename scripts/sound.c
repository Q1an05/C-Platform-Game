// sound.c
// 音效系统实现

#include "sound.h"
#include <stdio.h>
#include <stdlib.h>

// 音效文件路径
static const char* sound_files[SOUND_COUNT] = {
    "assets/sounds/jump.wav",       // SOUND_JUMP
    "assets/sounds/hurt.wav",       // SOUND_HURT
    "assets/sounds/power_up.wav",   // SOUND_POWER_UP
    "assets/sounds/coin.wav",       // SOUND_COIN
    "assets/sounds/explosion.wav",  // SOUND_EXPLOSION
    "assets/sounds/tap.wav"         // SOUND_TAP
};

// 背景音乐文件路径
static const char* music_file = "assets/sounds/music.mp3";

// 全局音效变量
static Mix_Chunk* sound_effects[SOUND_COUNT];
static Mix_Music* background_music = NULL;
static int sound_system_initialized = 0;

// 初始化音效系统
int init_sound_system() {
    // 初始化SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer初始化失败: %s\n", Mix_GetError());
        return 0;
    }
    
    // 设置混音通道数
    Mix_AllocateChannels(16);
    
    // 初始化音效数组
    for (int i = 0; i < SOUND_COUNT; i++) {
        sound_effects[i] = NULL;
    }
    
    // 加载音效文件
    for (int i = 0; i < SOUND_COUNT; i++) {
        sound_effects[i] = Mix_LoadWAV(sound_files[i]);
        if (!sound_effects[i]) {
            printf("无法加载音效文件 %s: %s\n", sound_files[i], Mix_GetError());
            // 继续加载其他音效，不要因为一个文件失败就退出
        } else {
            printf("成功加载音效: %s\n", sound_files[i]);
        }
    }
    
    // 加载背景音乐
    background_music = Mix_LoadMUS(music_file);
    if (!background_music) {
        printf("无法加载背景音乐 %s: %s\n", music_file, Mix_GetError());
    } else {
        printf("成功加载背景音乐: %s\n", music_file);
    }
    
    // 设置默认音量
    set_music_volume(64);   // 50% 音量
    set_sound_volume(96);   // 75% 音量
    
    sound_system_initialized = 1;
    printf("音效系统初始化成功！\n");
    return 1;
}

// 清理音效系统
void cleanup_sound_system() {
    if (!sound_system_initialized) return;
    
    // 停止所有音效和音乐
    Mix_HaltChannel(-1);
    Mix_HaltMusic();
    
    // 释放音效资源
    for (int i = 0; i < SOUND_COUNT; i++) {
        if (sound_effects[i]) {
            Mix_FreeChunk(sound_effects[i]);
            sound_effects[i] = NULL;
        }
    }
    
    // 释放背景音乐资源
    if (background_music) {
        Mix_FreeMusic(background_music);
        background_music = NULL;
    }
    
    // 关闭SDL_mixer
    Mix_CloseAudio();
    sound_system_initialized = 0;
    printf("音效系统已清理！\n");
}

// 播放指定音效
void play_sound(SoundEffect sound) {
    if (!sound_system_initialized) return;
    
    if (sound >= 0 && sound < SOUND_COUNT && sound_effects[sound]) {
        // 在任意可用通道播放音效
        if (Mix_PlayChannel(-1, sound_effects[sound], 0) == -1) {
            printf("播放音效失败: %s\n", Mix_GetError());
        }
    }
}

// 播放背景音乐
void play_background_music() {
    if (!sound_system_initialized || !background_music) return;
    
    if (!Mix_PlayingMusic()) {
        // 循环播放背景音乐 (-1 表示无限循环)
        if (Mix_PlayMusic(background_music, -1) == -1) {
            printf("播放背景音乐失败: %s\n", Mix_GetError());
        } else {
            printf("开始播放背景音乐\n");
        }
    }
}

// 停止背景音乐
void stop_background_music() {
    if (!sound_system_initialized) return;
    
    Mix_HaltMusic();
    printf("背景音乐已停止\n");
}

// 暂停背景音乐
void pause_background_music() {
    if (!sound_system_initialized) return;
    
    if (Mix_PlayingMusic() && !Mix_PausedMusic()) {
        Mix_PauseMusic();
        printf("背景音乐已暂停\n");
    }
}

// 恢复背景音乐
void resume_background_music() {
    if (!sound_system_initialized) return;
    
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
        printf("背景音乐已恢复\n");
    }
}

// 设置音乐音量
void set_music_volume(int volume) {
    if (!sound_system_initialized) return;
    
    // 确保音量在有效范围内 (0-128)
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    
    Mix_VolumeMusic(volume);
}

// 设置音效音量
void set_sound_volume(int volume) {
    if (!sound_system_initialized) return;
    
    // 确保音量在有效范围内 (0-128)
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    
    // 设置所有音效的音量
    for (int i = 0; i < SOUND_COUNT; i++) {
        if (sound_effects[i]) {
            Mix_VolumeChunk(sound_effects[i], volume);
        }
    }
}

// 检查音乐是否正在播放
int is_music_playing() {
    if (!sound_system_initialized) return 0;
    
    return Mix_PlayingMusic();
} 
// sound.h
// 音效系统头文件

#ifndef SOUND_H
#define SOUND_H

#include <SDL_mixer.h>

// 音效类型枚举
typedef enum {
    SOUND_JUMP,         // 跳跃音效
    SOUND_HURT,         // 受伤音效
    SOUND_POWER_UP,     // 获得技能音效
    SOUND_COIN,         // 通关音效
    SOUND_EXPLOSION,    // 击杀敌人音效
    SOUND_TAP,          // 菜单切换音效
    SOUND_COUNT         // 音效总数
} SoundEffect;

// 音效系统初始化和清理
int init_sound_system();       // 初始化音效系统
void cleanup_sound_system();   // 清理音效系统

// 音效播放控制
void play_sound(SoundEffect sound);    // 播放指定音效
void play_background_music();          // 播放背景音乐
void stop_background_music();          // 停止背景音乐
void pause_background_music();         // 暂停背景音乐
void resume_background_music();        // 恢复背景音乐

// 音量控制
void set_music_volume(int volume);     // 设置音乐音量 (0-128)
void set_sound_volume(int volume);     // 设置音效音量 (0-128)

// 音效状态查询
int is_music_playing();               // 检查音乐是否正在播放

#endif // SOUND_H 
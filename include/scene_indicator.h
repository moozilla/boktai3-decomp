#ifndef GUARD_SCENE_INDICATOR_H
#define GUARD_SCENE_INDICATOR_H

#include "global.h"
#include "scene_vec.h"

struct Vec {
    s16 x, y, z, pad;
};

struct SceneStatusIndicator {
    u32 flags;
    u8 unknown04[11];
    u8 mode;
    u8 unknown10[8];
    union {
        struct SceneVec vector;
        u32 words[2];
    } position;
    u8 unknown20[8];
    u16 state;
    u8 active;
    s8 slotIndex;
    u16 reset;
    u16 blinkTimer;
};

struct SceneStatusIndicatorPosition {
    u32 words[2];
};

struct SceneStatusIndicatorManager {
    u8 task[0x18];
    void *resource;
    struct SceneStatusIndicator entries[12];
    u32 allocated;
};

static inline u8 SceneStatusIndicator_IsAllocated(volatile u32 *allocated, s32 one, s32 index)
{
    if (*allocated & (one << index))
        return TRUE;
    return FALSE;
}

#endif

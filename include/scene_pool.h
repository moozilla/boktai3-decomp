#ifndef GUARD_SCENE_POOL_H
#define GUARD_SCENE_POOL_H
#include "engine.h"

/* Tentative pooled scene-motion controller, 0803CDF0 onward. Pool entries
 * occupy 0x17c bytes; the manager also owns four 0x8b0 actor slots.
 * State meanings remain provisional until the dispatcher's callers are mapped. */
struct ScenePoolEntry {
    u8 unknown0[0x0c];
    s32 scriptId;
    u32 index;
    s32 position[3];
    u32 unknown20;
    u8 active, kind;
    u8 unknown26[2];
    s32 duration, parameter;
    u32 origin, limit;
    u8 side, mode, repetitions;
    u8 unknown3b;
    u32 unknown3c;
    s32 motionPhase, stateFrame;
    s32 step, acceleration, speed, turnStep;
    s32 direction;
    u32 unknown5c;
    s32 idleFrame;
    u8 stateEntered;
    u8 unknown65[3];
    u32 state;
    u32 history[0x40];
    s32 settingA, delayA, settingB, delayB;
};
struct ScenePoolManager {
    u8 task[0x18];
    u32 capacity, activeCount;
    struct ScenePoolEntry *entries;
};
#endif

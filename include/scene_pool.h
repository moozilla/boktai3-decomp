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
    s32 motionPhase;
    u32 stateFrame;
    s32 step, acceleration, speed, turnStep;
    s32 direction; /* signed metric; exact geometric meaning is tentative */
    u32 unknown5c;
    s32 idleFrame;
    u8 stateEntered;
    u8 speedControlActive, speedControlDelay;
    s8 speedControlPulse;
    u32 state;
    u32 history[0x40]; /* per-state workspace; temporal history is not established */
    s32 settingA, delayA, settingB, delayB;
};
struct ScenePoolManager {
    u8 task[0x18];
    u32 capacity, activeCount;
    struct ScenePoolEntry *entries;
    u32 seenIds[8];
    u8 actors[4][0x8b0];
    u32 playerMask;
    s32 playerCount;
    u32 regions[2];
    u32 leftBounds[2], rightBounds[2];
    s32 playerPositions[2][4];
    u32 configuration, currentConfiguration;
    u32 firstId, lastId;
    u32 registry[3];
};
/* Partial actor view established by the frame/state callbacks in this range. */
struct ScenePoolActor {
    u8 unknown0[0x10];
    s32 position[4];
    u8 unknown20[0x18];
    s32 restrictionMetric;
    u8 unknown3c[0x1c];
    u16 timerA, timerB;
    u8 unknown5c[0x510];
    u32 motionFlags;
    u8 unknown570[0x3c];
    s32 forwardSpeed, verticalSpeed, lateralSpeed;
    u8 unknown5b8[0x34];
    s8 motionLock;
    u8 unknown5ed[0xf];
    u32 stateFlags;
    u8 unknown600[0x258];
    u32 controlFlags;
    u8 unknown85c[0x54];
};
/* Five signed thresholds used by the adjacent steering-state callbacks. */
struct ScenePoolSpeedBounds {
    s32 emergencyHigh, high, low, activeHigh, activeLow;
};
/* Position and flag consumers identify this separate player-pointer array.
 * This is not the gUnk_02000580 Player array from player.h. */
struct ScenePoolPlayer {
    u8 unknown0[0x28];
    s32 position[3];
    u8 unknown34[0x8c0];
    u16 flags;
};
extern struct ScenePoolPlayer *gUnk_020004F0[2];
#endif

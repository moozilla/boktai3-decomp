#ifndef GUARD_SCENE_POOL_H
#define GUARD_SCENE_POOL_H
#include "engine.h"
#include "scene_vec.h"

/* The copy helpers transfer 16 bytes; only x/y/z are interpreted.
 * The unnamed word is representation padding, not an uninitialized coordinate. */
struct ScenePoolVec32 {
    s32 x, y, z;
    u32 : 32;
};

/* Tentative pooled scene-motion controller, 0803CDF0 onward. Pool entries
 * occupy 0x17c bytes; the manager also owns four 0x8b0 actor slots.
 * State meanings remain provisional until the dispatcher's callers are mapped. */
struct ScenePoolEntry {
    u8 unknown0[0x0c];
    s32 scriptId;
    u32 index;
    struct ScenePoolVec32 position;
    u8 active, kind;
    u8 unknown26[2];
    s32 duration, parameter;
    u32 origin, limit;
    u8 side, mode, repetitions;
    u8 unknown3b;
    u32 unknown3c;
    s32 motionPhase; /* legacy name: target-player index, selected by 0803C114 */
    u32 stateFrame;
    s32 step, acceleration, speed, turnStep;
    s32 direction; /* legacy name: target-player x minus actor x (0803C114) */
    s32 unknown5c; /* target-player z minus actor z (0803C114) */
    s32 idleFrame;
    u8 stateEntered;
    u8 speedControlActive, speedControlDelay;
    s8 speedControlPulse;
    u32 state;
    u32 history[0x40]; /* per-state workspace; temporal history is not established */
    s32 settingA, delayA, settingB, delayB;
};
/* Contact volume used by the spatial index. The eight-byte vectors preserve
 * the initialization and contact-message copy widths. */
struct ScenePoolContact {
    u32 flags, mask, kind;
    u16 value;
    s8 contactState;
    u8 registrationFlags;
    struct SceneVec position, bounds, offset;
    u8 unknown28[9];
    s8 bucket;
    u8 unknown32[4], speed, unknown37;
    void *owner;
    void (*callback)(void);
};
/* Shared actor body, embedded after Task in the recoil actor. */
struct ScenePoolActor {
    u8 unknown0[0xc];
    u32 boundsFlags;
    struct ScenePoolVec32 position;
    u8 unknown20[0x18];
    s32 restrictionMetric;
    u8 unknown3c[0x1c];
    u16 timerA, timerB;
    u8 unknown5c[0xc];
    u32 unknown68;
    s32 contactType;
    u8 unknown70[0x24];
    s32 minimumForwardSpeed, maximumForwardSpeed;
    s32 minimumLateralSpeed, maximumLateralSpeed;
    s32 minimumForwardBias, maximumForwardBias;
    u8 contactActive, unknownAD[3];
    void *contactCallback;
    u8 unknownB4[0xC];
    s32 forwardBias; /* +0xc0, added to forwardSpeed (tentative) */
    u8 unknownc4[0x1f0];
    u8 emissionProfile[0xc]; /* observed descriptor prefix; full extent is unknown */
    u8 unknown2c0[0x48];
    u16 unknown308;
    u8 unknown30a[0x2E];
    u16 recoilResistance;
    u8 unknown33A[2];
    s32 biasClampMode;
    u8 unknown340[0x1E4];
    u16 stateSound, previousSound;
    u16 contactValue;
    u8 contactKind, contactPending;
    struct ScenePoolContact attackContact;
    struct ScenePoolContact bodyContact;
    s32 forwardSpeed, verticalSpeed, lateralSpeed;
    u8 unknown5b8[0x14];
    s32 recoilX, recoilY, recoilZ;
    u8 unknown5d8[0x14];
    s8 motionLock;
    u8 unknown5ed;
    u8 flags5ee;
    u8 unknown5ef[5];
    struct ScenePoolPlayer *linkedPlayer; /* +0x5f4, restriction flag consumer 0811628C */
    u8 unknown5f8[4];
    u32 stateFlags;
    u8 unknown600[0x18];
    s16 collisionWidth, collisionHeight;
    u8 unknown61C[0x23C];
    u32 controlFlags;
    u8 unknown85c[0x44];
    struct ScenePoolVec32 savedPosition;
};
struct ScenePoolManager {
    u8 task[0x18];
    u32 capacity, activeCount;
    struct ScenePoolEntry *entries;
    u32 seenIds[8];
    struct ScenePoolActor actors[4];
    u32 playerMask;
    s32 playerCount;
    u32 regions[2];
    u32 leftBounds[2], rightBounds[2];
    s32 playerPositions[2][4];
    u32 configuration, currentConfiguration;
    u32 firstId, lastId;
    u32 registry[3];
};
/* Five signed thresholds used by the adjacent forward-speed callbacks. */
struct ScenePoolSpeedBounds {
    s32 emergencyHigh, high, low, activeHigh, activeLow;
};
/* Position and flag consumers identify this separate player-pointer array.
 * This is not the gPlayers Player array from player.h. */
struct ScenePoolPlayerDraw {
    u8 unknown0[0x16];
    u16 flags;
};
struct ScenePoolPlayer {
    u8 unknown0[0x28];
    s32 position[3];
    u8 unknown34[0x5c0];
    s32 viewportOriginX; /* +0x5f4, rounded down to a pixel by 0811AB78 */
    u8 unknown5f8[0x26c];
    u32 gloveTimer; /* +0x864, tentative Glove controller state */
    u8 gloveActive;
    u8 unknown869[3];
    void (*gloveUpdate)(void);
    u8 unknown870[0x58];
    struct ScenePoolPlayerDraw gloveDraw; /* +0x8c8 */
    u8 unknown8e0[0x14];
    u16 flags;
};
/* 0x020004F0 */
extern struct ScenePoolPlayer *gParticipantActors[2];
#endif

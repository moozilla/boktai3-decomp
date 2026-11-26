#ifndef GUARD_AREA_EFFECT_H
#define GUARD_AREA_EFFECT_H
#include "system/game_state/game_state.h"
#include "engine/engine_globals.h"
#include "engine/task.h"
#include "player/player.h"
/* Tentative identity: scripted rectangular areas with optional display sprites. */
struct AreaEffectEntry {
    struct SceneVec position;
    u32 spriteFlags;
    u8 unknownC[3];
    u8 renderMode;
    u8 unknown10[8];
    u16 priority;
    u8 unknown1A[10];
    struct SceneVec displayPosition;
    u8 unknown2C[0x34];
    u8 graphics[2][0x1C];
    u16 tag;
    u8 enabled, unknown9B;
    u8 kind, hasSprite;
    u16 cachedValue;
    u16 xMin, zMin, xMax, zMax;
};
struct AreaEffectManager {
    struct Task task;
    struct Player *player;
    u16 unknown1C, resourceFraction, idleFrame, actionFrame, graphicsId;
    u8 count, mode, visible, override, unknown2A[2];
    struct AreaEffectEntry entries[16];
    u16 resourceFractions[4];
};
/* 0x0200010C */
extern struct AreaEffectManager *gAreaEffectManager;
extern const u16 gUnk_08605A78[], gUnk_08605A80[];
extern u32 gUnk_02000560;
void AreaEffect_SetBounds(struct AreaEffectEntry *, s32, struct SceneVec *, s32);
void AreaEffect_AccumulatePlayerResource(struct AreaEffectManager *, s32);
#endif

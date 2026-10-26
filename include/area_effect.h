#ifndef GUARD_AREA_EFFECT_H
#define GUARD_AREA_EFFECT_H
#include "game_state.h"
/* Tentative identity: scripted rectangular areas with optional display sprites. */
struct AreaEffectPosition {
    s16 x, y, z, pad;
};
struct AreaEffectEntry {
    struct AreaEffectPosition position;
    u32 spriteFlags;
    u8 unknownC[3];
    u8 renderMode;
    u8 unknown10[8];
    u16 priority;
    u8 unknown1A[10];
    struct AreaEffectPosition displayPosition;
    u8 unknown2C[0x34];
    u8 graphics[2][0x1C];
    u16 tag;
    u8 enabled, unknown9B;
    u8 kind, hasSprite;
    u16 cachedValue;
    u16 xMin, zMin, xMax, zMax;
};
struct AreaEffectPlayer {
    u8 unknown0[0x1C];
    u32 stateTag, flags; /* stateTag interpretation is tentative; active value is1 */
    u8 unknown24[12];
    struct AreaEffectPosition position;
    u8 unknown38[0x3E0];
    u8 actionMode;
    u8 unknown419[0x3E];
    u8 action, actionPhase;
};
struct AreaEffectManager {
    u8 unknown0[0x18];
    struct AreaEffectPlayer *player;
    u16 unknown1C, resourceFraction, idleFrame, actionFrame, graphicsId;
    u8 count, mode, visible, override, unknown2A[2];
    struct AreaEffectEntry entries[16];
    u16 resourceFractions[4];
};
extern struct AreaEffectManager *gAreaEffectManager;
extern const u16 gUnk_08605A78[], gUnk_08605A80[];
extern u32 gEngineFlags, gUnk_02000560;
extern u16 gUnk_03005260;
void AreaEffect_SetBounds(struct AreaEffectEntry *, s32, struct AreaEffectPosition *, s32);
void AreaEffect_AccumulatePlayerResource(struct AreaEffectManager *, s32);
#endif

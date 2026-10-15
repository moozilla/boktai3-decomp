#ifndef GUARD_STAGED_EFFECT_H
#define GUARD_STAGED_EFFECT_H
#include "global.h"
/* Tentative identity: a five-sprite scripted burst/transition effect. */
struct StagedEffectSprite {
    u32 flags;
    u8 unknown4[4];
    u8 scaleX, scaleY;
    u8 unknownA[0x12];
    u16 x, y, z, unknown22;
    u8 unknown24[8];
};
struct StagedEffect {
    u8 unknown0[0x4C];
    u16 hasScript, state, paletteIndex, scale;
    u32 scriptReleaseFrame;
    s16 travel, initialTravel, spread, initialSpread;
    s16 shakeX, shakeZ;
    u32 frame;
    void *pendingScript;
    u32 positionXY, positionZ;
    struct StagedEffectSprite sprites[5];
    u8 unknown150[0x1C];
    const u16 *activePalette;
    u16 blendedPalette[16];
};
extern u16 *gUnk_030042E4;
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void sub_0804D874(struct StagedEffect *);
void sub_0804D890(struct StagedEffect *);
void sub_0804D778(struct StagedEffect *, s32, s32, s32);
void sub_0822B2F8(u32);
#endif

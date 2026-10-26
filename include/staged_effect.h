#ifndef GUARD_STAGED_EFFECT_H
#define GUARD_STAGED_EFFECT_H
#include "global.h"
/* Tentative identity: a five-sprite scripted burst/transition effect. */
struct StagedEffectSprite {
    u32 flags;
    u8 unknown4[3];
    u8 renderMode;
    u8 scaleX, scaleY;
    u8 unknownA[6];
    u16 priority;
    u8 unknown12[10];
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
    u32 pendingScriptId;
    union {
        u32 words[2];
        struct {
            u16 x, y, z, pad;
        } axes;
    } position;
    struct StagedEffectSprite sprites[5];
    union {
        u8 bytes[0x1C];
        struct {
            u8 unknown0[12];
            const u16 *palette;
        } view;
    } graphics;
    const u16 *activePalette;
    u16 blendedPalette[16];
};
extern u16 *gPaletteBankData;
extern u16 gUnk_0203B400[];
extern u32 gRandomIndex;
void StagedEffect_MarkOuterSprites(struct StagedEffect *);
void StagedEffect_MarkOuterSpritesAlternate(struct StagedEffect *);
void StagedEffect_BlendPalette(struct StagedEffect *, s32, s32, s32);
void Sound_PlaySong(u32);
#endif

#ifndef GUARD_LINK_PLAYER_PALETTE_H
#define GUARD_LINK_PLAYER_PALETTE_H

#include "global.h"

struct LinkPalTarget {
    u8 pad0[6];
    u16 paletteBank;
    u8 pad8[4];
    u16 *palette;
};
struct LinkPlayerFade {
    u8 pad0[0x18];
    s32 slot;
    u8 pad1C[4];
    u32 statusFlags;
    u8 pad24[0xc6 - 0x24];
    u16 paletteBank;
    u8 padC8[0xd4 - 0xc8];
    u16 *palette;
    u8 padD8[0x178 - 0xd8];
    struct LinkPalTarget *target;
    u8 pad17C[0x360 - 0x17c];
    u16 basePalette[16];
    u16 workPalette[16];
    u8 pad3A0[0x419 - 0x3a0];
    u8 alternate;
    u8 pad41A[0xab0 - 0x41a];
    u16 paletteBanks[10];
    s16 modeArg;
    u8 brightness;
    u8 pad_ac7;
    u8 animIdx;
    u8 animIdxNew;
    u8 padACA[0xad6 - 0xaca];
    u16 srcSlot;
    u16 fadeIn;
    u16 dstSlot;
    u16 fadeOut;
};

void LinkPlayer_InitAnimationSlots(struct LinkPlayerFade *actor);
void LinkPlayer_InitAnimationState(struct LinkPlayerFade *actor);
void sub_08239A20(struct LinkPlayerFade *actor, s32 mode);
void LinkPlayer_BlendPalette(struct LinkPlayerFade *actor, const u16 *source, s32 sourceWeight,
                             s32 baseWeight, s32 shift);
void LinkPlayer_LoadAnimationParameters(struct LinkPlayerFade *actor);
s32 ActorEffect_SelectModeFlag(struct LinkPlayerFade *actor);
void PaletteFade_ApplySelectiveBrightness(struct LinkPlayerFade *actor);

#endif

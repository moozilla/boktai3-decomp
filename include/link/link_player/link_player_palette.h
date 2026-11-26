#ifndef GUARD_LINK_PLAYER_PALETTE_H
#define GUARD_LINK_PLAYER_PALETTE_H

#include "global.h"

struct LinkPalTarget {
    u8 pad0[0xc];
    u16 *palette;
};
struct LinkPlayerFade {
    u8 pad0[0xd4];
    u16 *palette;
    u8 padD8[0x178 - 0xd8];
    struct LinkPalTarget *target;
    u8 pad17C[0x360 - 0x17c];
    u16 basePalette[16];
    u16 workPalette[16];
    u8 pad3A0[0x419 - 0x3a0];
    u8 alternate;
    u8 pad41A[0xac4 - 0x41a];
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

void LinkPlayer_BlendPalette(struct LinkPlayerFade *actor, const u16 *source, s32 sourceWeight,
                             s32 baseWeight, s32 shift);

#endif

#ifndef GUARD_INCLUDE_BOSSES_NIDHOGGR_NIDHOGGR_POOL_SPRITE_LAYOUT_H
#define GUARD_INCLUDE_BOSSES_NIDHOGGR_NIDHOGGR_POOL_SPRITE_LAYOUT_H

#include "global.h"
#include "bosses/nidhoggr/nidhoggr_effect_motion_layout.h"

struct NidhoggrPoolSprite {
    u32 flags;
    u8 pad4[0xc];
    u16 frame;
    u8 pad12[0xa];
    struct NidhoggrEffectMotion position; /* confidence=low */
    u8 pad24[0x8];
};

typedef char NidhoggrPoolSpriteSizeCheck[sizeof(struct NidhoggrPoolSprite) == 0x2c ? 1 : -1];

#endif

#ifndef GUARD_INCLUDE_PLAYER_EFFECTS_SPIRAL_TRAIL_SPRITE_H
#define GUARD_INCLUDE_PLAYER_EFFECTS_SPIRAL_TRAIL_SPRITE_H

#include "global.h"

struct SpiralTrailSprite {
    u32 flags; /* confidence=low */
    u8 pad4[0x44];
    u16 *src;
    u16 dest[16]; /* confidence=low */
    u8 b6c; /* confidence=low */
    u8 alpha; /* confidence=low */
    u8 pad6E[0x2];
};

typedef char SpiralTrailSpriteSizeCheck[sizeof(struct SpiralTrailSprite) == 0x70 ? 1 : -1];

#endif

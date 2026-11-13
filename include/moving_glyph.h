#ifndef GUARD_MOVING_GLYPH_H
#define GUARD_MOVING_GLYPH_H

#include "global.h"

struct MovingSpritePosition {
    u16 x, unknown2, y, unknown6;
};
union MovingSpritePositionValue {
    struct MovingSpritePosition coordinates;
    struct { u32 a, b; } words;
};
struct MovingSprite {
    u8 task[0x18];
    u32 flags;
    u8 spritePrefix[0x18];
    struct MovingSpritePosition displayedPosition;
    u8 spriteSuffix[8];
    u8 animation[0x1C];
    union MovingSpritePositionValue position;
    u8 unknown68[2];
    u16 id;
    u16 angle;
    s16 velocityX, velocityY;
    u16 delay, duration, frame;
    void (*phase)(void);
};

#endif

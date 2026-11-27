#ifndef GUARD_INCLUDE_UI_BOOKS_BESTIARY_HOMING_SPRITE_H
#define GUARD_INCLUDE_UI_BOOKS_BESTIARY_HOMING_SPRITE_H

#include "global.h"

struct BestiaryHomingSprite {
    u8 pad0[0x18];
    s16 x;
    s16 y;
    s16 z;
    u8 pad1E[0x2];
};

typedef char BestiaryHomingSpriteSizeCheck[sizeof(struct BestiaryHomingSprite) == 0x20 ? 1 : -1];

#endif

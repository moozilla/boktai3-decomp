#ifndef GUARD_INCLUDE_EFFECTS_BURSTS_FRAGMENT_BURST_FRAGMENT_SPRITE_LAYOUT_H
#define GUARD_INCLUDE_EFFECTS_BURSTS_FRAGMENT_BURST_FRAGMENT_SPRITE_LAYOUT_H

#include "global.h"
#include "scene/collision/contact_position.h"

struct FragmentSprite {
    u32 flags;
    u8 active; /* confidence=low */
    u8 pad5[0xb];
    u16 frame; /* confidence=low */
    u8 pad12[0xa];
    struct ContactPosition position; /* confidence=low */
    u8 pad24[0x8];
};

typedef char FragmentSpriteSizeCheck[sizeof(struct FragmentSprite) == 0x2c ? 1 : -1];

#endif

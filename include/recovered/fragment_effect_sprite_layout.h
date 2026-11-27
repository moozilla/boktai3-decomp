#ifndef GUARD_INCLUDE_RECOVERED_FRAGMENT_EFFECT_SPRITE_LAYOUT_H
#define GUARD_INCLUDE_RECOVERED_FRAGMENT_EFFECT_SPRITE_LAYOUT_H

#include "global.h"
#include "scene/collision/contact_position.h"

struct FragmentEffectSprite {
    u32 flags;
    u8 active;
    u8 pad5[0x2];
    u8 priority; /* confidence=low */
    s8 scaleX; /* confidence=low */
    s8 scaleY; /* confidence=low */
    u8 padA[0x2];
    void *graphics; /* confidence=low */
    u16 frame; /* confidence=low */
    u8 pad12[0xa];
    struct ContactPosition position; /* confidence=low */
    u8 pad24[0x8];
};

typedef char FragmentEffectSpriteSizeCheck[sizeof(struct FragmentEffectSprite) == 0x2c ? 1 : -1];

#endif

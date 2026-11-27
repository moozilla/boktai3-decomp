#ifndef GUARD_INCLUDE_ENGINE_VIDEO_SPRITE_COMPONENT_H
#define GUARD_INCLUDE_ENGINE_VIDEO_SPRITE_COMPONENT_H

#include "global.h"

struct SpriteComponent {
    void (*update)(struct SpriteComponent *); /* confidence=low */
    void (*destroy)(struct SpriteComponent *); /* confidence=low */
    u32 flags;
    u8 padC[0x44];
    u32 position[2]; /* confidence=low */
    s16 frame;
    s16 collisionStatus; /* confidence=low */
};

typedef char SpriteComponentSizeCheck[sizeof(struct SpriteComponent) == 0x5c ? 1 : -1];

#endif

#ifndef GUARD_INCLUDE_ENGINE_VIDEO_SPRITE_COMPONENT_H
#define GUARD_INCLUDE_ENGINE_VIDEO_SPRITE_COMPONENT_H

#include "global.h"
#include "engine/video/sprite.h"
#include "scene/collision/contact_collider.h"

struct SpriteComponent {
    void (*update)(struct SpriteComponent *);  /* confidence=low */
    void (*destroy)(struct SpriteComponent *); /* confidence=low */
    struct DisplaySequenceSprite sprite;       /* 0x08 */
    struct SpriteRenderResource resource;      /* 0x34 */
    struct ContactPosition position;           /* 0x50: confidence=low */
    s16 frame;
    s16 collisionStatus;              /* confidence=low */
    struct ContactCollider collider;  /* 0x5C */
};

typedef char SpriteComponentSizeCheck[sizeof(struct SpriteComponent) == 0xb0 ? 1 : -1];

#endif

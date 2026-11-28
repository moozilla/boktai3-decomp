#ifndef GUARD_ACTOR_MARKER_POOL_H
#define GUARD_ACTOR_MARKER_POOL_H

#include "global.h"
#include "effects/projectiles/contact_swarm_sprite.h"
#include "engine/video/display_sequence.h"
#include "engine/ram.h"

/* Tentative: a singleton task drawing up to eight marker sprites that follow actors. */

struct FixedPosition {
    s32 x, y, z;
};

struct MarkerSlot {
    u8 resource[0x1c];
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    const struct FixedPosition *anchor; /* position the marker follows */
    u32 active;
};

struct MarkerPool {
    u8 task[0x18];
    struct MarkerSlot slots[8];
    void *spriteSheet;
};

#endif

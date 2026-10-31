#ifndef GUARD_TRANSIENT_SPRITE_EFFECTS_H
#define GUARD_TRANSIENT_SPRITE_EFFECTS_H
#include "contact_swarm.h"
/* Sixteen 0x58 transient sprite slots in a 0x598 list-10 singleton. */
struct TransientEffectSprite {
    u32 flags;
    u8 unknown4[3], priority;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[10];
    union {
        struct ContactPackedVector bits;
        struct ContactPosition vector;
    } position;
    u8 unknown24[8];
};
struct TransientSpriteEffect {
    u8 active, options;
    u16 age, dragDelay, drag;
    struct ContactPosition velocity;
    struct ContactSwarmGraphics graphics;
    struct TransientEffectSprite sprite;
};
struct TransientSpriteEffects {
    u8 task[0x18];
    struct TransientSpriteEffect slots[16];
};
/* 0x020000A0 */
extern struct TransientSpriteEffects *gTransientSpriteEffects;
#endif

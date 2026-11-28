#ifndef GUARD_LINKED_EFFECT_H
#define GUARD_LINKED_EFFECT_H

#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/video/display_sequence.h"
#include "effects/bursts/fragment_effect_sprite_layout.h"
#include "effects/bursts/spiral_burst.h"
#include "scene/collision/contact_collider.h"

struct LinkedEffect {
    struct FragmentEffectSprite sprite;
    u8 graphics[0x1C];
    struct DisplaySequenceState animation;
    struct ContactCollider colliders[2];
    struct ContactPosition *target;
    u8 unk_104[0x68];
    struct ContactPosition velocity;
    u16 unk_174;
    u8 state;
    s8 slot;
    u16 frame;
    u8 entered, speed, animationEnded, heading;
    u16 unk_17E, lifetime, riseTimer, delay, turnTimer, maxSpeed;
};

struct LinkedEffectPool {
    struct Task task;
    void *sequences;
    u32 occupied;
    struct LinkedEffect effects[8];
};

static inline u8 LinkedEffectPool_IsOccupied(struct LinkedEffectPool *pool, s32 slot)
{
    if (pool->occupied & (1 << slot))
        return TRUE;
    return FALSE;
}

static inline void LinkedEffect_Subtract(struct ContactPosition *out,
                                         const struct ContactPosition *from,
                                         const struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

extern const s16 gUnk_086149C4[];

/* Horizontal vector of the given length along a 0..255 heading. */
static inline void LinkedEffect_SetPlanarVector(struct ContactPosition *out, u8 heading, s32 length)
{
    out->x = SpiralBurst_ScaleOffset(length * gUnk_086149C4[(heading + 64) & 255]);
    out->y = 0;
    out->z = SpiralBurst_ScaleOffset(length * gUnk_086149C4[heading]);
}

static inline void LinkedEffect_AddPosition(struct ContactPosition *out,
                                            const struct ContactPosition *origin)
{
    out->x = origin->x + out->x;
    out->y = origin->y + out->y;
    out->z = origin->z + out->z;
}

#endif

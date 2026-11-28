#ifndef GUARD_LINKED_EFFECT_H
#define GUARD_LINKED_EFFECT_H

#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/video/display_sequence.h"
#include "effects/bursts/fragment_effect_sprite_layout.h"
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

#endif

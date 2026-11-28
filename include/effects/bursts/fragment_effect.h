#ifndef GUARD_FRAGMENT_EFFECT_H
#define GUARD_FRAGMENT_EFFECT_H

#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/video/display_sequence.h"
#include "effects/bursts/fragment_effect_sprite_layout.h"
#include "effects/bursts/spiral_burst.h"
#include "scene/collision/contact_collider.h"

struct Fragment {
    struct FragmentEffectSprite sprite;
    struct DisplaySequenceState sequence;
    struct ContactCollider collider;
    struct ContactPosition center, offset;
    u32 angle, angularSpeed;
    s16 direction;
    u16 unk_AA;
    s16 radius, scaleTimer;
    void *sequenceResource;
    s32 lifetime;
    u16 unk_B8;
    u8 state;
    s8 slot;
    u16 timer;
    u8 entered;
};

struct FragmentEffectPool {
    struct Task task;
    u8 graphics[0x20];
    u32 occupied;
    struct Fragment fragments[8];
};

static inline u8 FragmentEffectPool_IsOccupied(struct FragmentEffectPool *pool, s32 slot)
{
    if (pool->occupied & (1 << slot))
        return TRUE;
    return FALSE;
}

extern const s16 gUnk_086149C4[];

/* Point on the orbit circle for a 20.12 fixed-point angle (0x100000 is a full turn). */
static inline void FragmentEffect_ComputeOffset(struct ContactPosition *out, u32 heading,
                                                s32 radius)
{
    u32 angle = heading >> 12;
    s32 scale = radius * 128;

    out->x = SpiralBurst_ScaleOffset(scale * gUnk_086149C4[(angle + 64) & 255]);
    out->y = 0;
    out->z = SpiralBurst_ScaleOffset(scale * gUnk_086149C4[angle & 255]);
}

static inline void FragmentEffect_AddPositions(struct ContactPosition *out,
                                               const struct ContactPosition *a,
                                               const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

#endif

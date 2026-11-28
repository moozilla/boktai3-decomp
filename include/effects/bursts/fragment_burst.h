#ifndef GUARD_FRAGMENT_BURST_H
#define GUARD_FRAGMENT_BURST_H

#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/video/sprite.h"
#include "engine/video/display_sequence.h"
#include "scene/collision/contact_collider.h"
#include "effects/scene_effect_projection.h"

struct FragmentBurst {
    struct DisplaySequenceSprite sprites[4];
    struct DisplaySequenceState sequences[4];
    struct ContactPosition origin, position;
    struct ContactCollider collider;
    u16 age;
    u8 state;
    s8 slot;
    u32 completionScript;
    u16 timer;
    u8 enter;
};

struct FragmentBurstPool {
    struct Task task;
    u8 graphics[0x1C];
    void *sequenceResource;
    u32 occupied;
    struct FragmentBurst bursts[4];
};

static inline u8 FragmentBurstPool_IsOccupied(struct FragmentBurstPool *pool, s32 slot)
{
    if (pool->occupied & (1 << slot))
        return TRUE;
    return FALSE;
}

static inline void FragmentBurst_ProjectToScreen(struct ContactPosition *screen,
                                                 const struct ContactPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum, height;

    screen->x = SceneEffect_ScaleProjection(3 * (x - z));
    sum = SceneEffect_ScaleProjection(3 * (x + z));
    height = SceneEffect_ScaleProjection(3 * world->y);
    screen->y = sum - height;
    screen->z = sum + height;
    screen->x = screen->x - gCameraOrigin[0] + 120;
    screen->y = screen->y - gCameraOrigin[1] + 80;
    screen->z -= gCameraOrigin[2];
}

#endif

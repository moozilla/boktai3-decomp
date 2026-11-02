#ifndef GUARD_SCENE_ROUTE_SAMPLE_H
#define GUARD_SCENE_ROUTE_SAMPLE_H

#include "contact_position.h"
#include "monster_particle_random.h"

extern const s16 gUnk_086149C4[];

static inline s32 SceneRoute_ScaleComponent(s32 component)
{
    if (component >= 0)
        return component >> 12;
    return -((-component) >> 12);
}

/* Preserve the sampling origin for the next route draw while projecting this one. */
static inline void SceneRoute_ProjectRandomOffset(struct ContactPosition *out, u32 angle, s32 speed,
                                                 u32 origin, u32 *savedOrigin)
{
    const s16 *sine = gUnk_086149C4;
    s32 x, z;
    s32 sample;

    *savedOrigin = origin;
    sample = sine[(angle + 64) & 255];
    x = sample * speed;
    out->x = SceneRoute_ScaleComponent(x);
    out->y = 0;
    z = gUnk_086149C4[angle & 255] * speed;
    out->z = SceneRoute_ScaleComponent(z);
}

#endif

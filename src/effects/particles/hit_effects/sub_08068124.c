#include "effects/particles/scene_particle_burst.h"
s32 SceneParticleBurst_CreateWithRanges(const struct ContactPosition *, const struct ContactPosition *,
                 const struct ContactPosition *, s32, u32, u32, u32);
s32 SceneParticleBurst_CreateWhilePaused(const struct ContactPosition *position,
                                         const struct ContactPosition *range,
                                         const struct ContactPosition *jitter, s32 count,
                                         u32 priority, u32 flip)
{
    return SceneParticleBurst_CreateWithRanges(position, range, jitter, count, priority, flip, 0);
}

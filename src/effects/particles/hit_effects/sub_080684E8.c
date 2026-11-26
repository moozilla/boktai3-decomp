#include "effects/particles/scene_particle_pool.h"
s32 SceneParticle_CreateAnchoredRanges(const struct ContactPosition *, const struct ContactPosition *,
                 const struct ContactPosition *, const struct ContactPosition *, s32, u32);
s32 SceneParticle_CreateDefaultPriority(const struct ContactPosition *anchor,
                                        const struct ContactPosition *offset,
                                        const struct ContactPosition *range,
                                        const struct ContactPosition *jitter, s32 count)
{
    return SceneParticle_CreateAnchoredRanges(anchor, offset, range, jitter, count, 2);
}

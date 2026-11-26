#include "effects/particles/scene_particle_pool.h"
s32 SceneParticle_CreateAnchoredRanges(const struct ContactPosition *, const struct ContactPosition *,
                 const struct ContactPosition *, const struct ContactPosition *, s32, u32);
s32 SceneParticle_CreateParticles(const struct ContactPosition *anchor,
                                  const struct ContactPosition *offset,
                                  const struct ContactPosition *range,
                                  const struct ContactPosition *jitter, s32 count, u32 priority)
{
    return SceneParticle_CreateAnchoredRanges(anchor, offset, range, jitter, count, priority);
}

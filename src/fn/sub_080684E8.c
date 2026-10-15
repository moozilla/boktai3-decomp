#include "scene_particle_pool.h"
#define SceneParticle_CreateDefaultPriority sub_080684E8
s32 sub_08068500(const struct ContactPosition *, const struct ContactPosition *,
                 const struct ContactPosition *, const struct ContactPosition *, s32, u32);
s32 SceneParticle_CreateDefaultPriority(const struct ContactPosition *anchor,
                                        const struct ContactPosition *offset,
                                        const struct ContactPosition *range,
                                        const struct ContactPosition *jitter, s32 count)
{
    return sub_08068500(anchor, offset, range, jitter, count, 2);
}

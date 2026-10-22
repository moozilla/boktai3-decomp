#include "global.h"
struct SceneParticle;
struct SceneParticleVector;
struct SceneParticle *SceneParticle_Create(struct SceneParticleVector *position,
                                           struct SceneParticleVector *velocity, u32 lifetime,
                                           u32 kind, u32 mode);

struct SceneParticle *sub_0812D7A8(struct SceneParticleVector *position,
                                   struct SceneParticleVector *velocity, u32 lifetime, u32 kind)
{
    return SceneParticle_Create(position, velocity, lifetime, kind, 1);
}
